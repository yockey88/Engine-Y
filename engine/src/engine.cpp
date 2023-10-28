#include "engine.hpp"

#include <string>
#include <queue>
#include <thread>

#include "core/app.hpp"
#include "core/filesystem.hpp"
#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "event/event_manager.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene.hpp"
#include "physics/physics_engine.hpp"
#include "scripting/script_engine.hpp"

namespace YE {
    
    WindowConfig App::GetWindowConfig() {
        return WindowConfig{};
    }

    Engine::Engine() {
        logger = Logger::Instance();
        logger->OpenLog();
        Filesystem::Initialize();
        
        task_manager = TaskManager::Instance();
        resource_handler = ResourceHandler::Instance();
        event_manager = EventManager::Instance();
        renderer = Renderer::Instance(); 
        script_engine = ScriptEngine::Instance();
        physics_engine = PhysicsEngine::Instance();

        Systems::Initialize();
    }

    Engine* Engine::singleton = nullptr;
    
    std::filesystem::path Engine::FindProjectFile() {
        YE_CRITICAL_ASSERTION(app != nullptr , "Application is null");

        std::string project_name = app->ProjectName();
        std::string file_name = project_name;

        std::vector<std::string> extensions = { "yproj" , ".ysc" , ".ys" , ".y" };
        std::filesystem::path path = project_name;
        path /= file_name;
        for (uint32_t i = 0; i < extensions.size(); ++i) {
            path.replace_extension(extensions[i]);
            if (std::filesystem::exists(path))
                return path;
        }

        return std::filesystem::path();
    }

    void Engine::InitializeSubSytems() {
        task_manager->DispatchTask([&](){ 
            stats = ynew EngineStats;
            for (uint32_t i = 0; i < kFrameTimeBufferSize; ++i) 
                stats->frame_times[i] = 0.0f;
        });

        event_manager->RegisterShutdownCallback([&](ShutdownEvent*) -> bool { 
            this->HandleShutdownEvent();
            return true;
        });
        
        task_manager->DispatchTask([](){ Keyboard::Initialize(); });
        task_manager->DispatchTask([](){ Mouse::Initialize(); });

        script_engine->Initialize();
        script_engine->LoadProjectModules();

        renderer->Initialize(app);
        renderer->OpenWindow();

        resource_handler->Load();
       
    }
    
    void Engine::Update(float dt) {
        event_manager->PollEvents();
        Keyboard::Update();
        Mouse::Update();
        app->Update(dt);
    }
    
    void Engine::HandleShutdownEvent() {
        running = false;
    }

    Engine* Engine::Instance() {
        if (singleton == nullptr) {
            singleton = ynew Engine;
        }
        return singleton;
    }

    void Engine::RegisterApplication(App* app) {
        this->app = app;
        if (app == nullptr) {
            YE_ERROR("Application is null");
            return;
        }

        app_config = app->GetEngineConfig();

        if (app_config.use_project_file) {
            std::filesystem::path project_path = FindProjectFile();
            if (project_path.empty()) {
                YE_ERROR("Project file missing");
                return;
            }

            YE::YScriptLexer lexer(project_path.string());

            auto [src , tokens] = lexer.Lex();
            project_file_src = src;

            YE::YScriptParser parser(tokens);
            project_ast = parser.Parse();
        }

        
        app_loaded = true;
    }

    void Engine::Initialize() {
        this->InitializeSubSytems();

        if (app_config.use_project_file && app_loaded) {
            YE::YS::Interpreter interpreter(project_ast);
            project_scene_graph = interpreter.BuildScene();
        }

        if (!app->Initialize()) {
            YE_ERROR("Failed to initialize application");
            return;
        }
        running = true;
    }
    
    void Engine::Run() {
        delta_time.Start();
        Mouse::SnapToCenter();

        task_manager->FlushTasks();
        while (running) {
            float dt = frame_rate.TimeStep();
            task_manager->DispatchTask([dt = &delta_time , stats = stats](){
                for (uint32_t i = 0; i < kFrameTimeBufferSize - 1; ++i)
                    stats->frame_times[i] = stats->frame_times[i + 1];
                stats->frame_times[kFrameTimeBufferSize - 1] = dt->Get();
            });

            Update(dt);
            task_manager->FlushTasks();

            renderer->Render();
            event_manager->FlushEvents();
            
            frame_rate.Enforce();
        }
    }

    void Engine::Shutdown() {
        task_manager->FlushTasks();

        app->Shutdown();
        if (app != nullptr) ydelete app;

        Systems::Teardown();

        resource_handler->Offload();

        renderer->CloseWindow();
        script_engine->Shutdown();
        physics_engine->Cleanup();

        resource_handler->Cleanup();
        event_manager->Cleanup();
        task_manager->Cleanup();
        renderer->Cleanup();
        script_engine->Cleanup();
        
        YE_INFO("Goodbye");
        logger->CloseLog();
        if (singleton != nullptr) ydelete singleton;
    }

}