#include "engine.hpp"

#include <string>
#include <queue>
#include <thread>
#include <filesystem>

#include "core/app.hpp"
#include "core/filesystem.hpp"
#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "core/event_manager.hpp"
#include "project/project_manager.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene_manager.hpp"
#include "physics/physics_engine.hpp"
#include "scripting/script_engine.hpp"

namespace YE {
    
    Engine::Engine() {
        logger = Logger::Instance();
        logger->OpenLog();    
    }

    Engine::~Engine() {
        logger->CloseLog();
        logger = nullptr;
    }

    Engine* Engine::singleton = nullptr;
        
    bool Engine::ValidateConfiguration() {
        ENTER_FUNCTION_TRACE();

        if (!Filesystem::FileExists(app_config.project_path)) {
            ENGINE_ERROR("Project directory does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.project_file)) {
            ENGINE_ERROR("Project file does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.modules_directory)) {
            ENGINE_ERROR("Modules directory does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.mono_config_path)) {
            ENGINE_ERROR("Mono config file does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.engine_root)) {
            ENGINE_ERROR("Engine root directory does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.MonoDllPath())) {
            ENGINE_ERROR("Mono dll directory does not exist");
            return false;

        }

        EXIT_FUNCTION_TRACE();
    }

    void Engine::InitializeSubSytems() { 
        ENTER_FUNCTION_TRACE();

        task_manager = TaskManager::Instance();
        resource_handler = ResourceHandler::Instance();
        event_manager = EventManager::Instance();
        renderer = Renderer::Instance(); 
        script_engine = ScriptEngine::Instance();
        physics_engine = PhysicsEngine::Instance();
        scene_manager = SceneManager::Instance();
        
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

        renderer->Initialize(app , app_config.window_config);

        Systems::Initialize();
        
        resource_handler->Load(); 

        if (app_config.use_project_file) {
            Scene* curr_scene = project_manager->LoadProjectFile(app_config.project_file);

            if (curr_scene == nullptr) {
                ENGINE_ERROR("Failed to load project file");
                // scene_manager->LoadDefaultScene();
            } else {
                scene_manager->AddScene(curr_scene);
            }
        } else {
            ENGINE_INFO("No project file specified");
            // scene_manager->LoadDefaultScene();
        } 

        EXIT_FUNCTION_TRACE();
    }
    
    void Engine::Update(float dt) {
        event_manager->PollEvents();
        Keyboard::Update();
        Mouse::Update();
        app->Update(dt);
    }
    
    void Engine::HandleShutdownEvent() {
        ENTER_FUNCTION_TRACE();
        running = false;
        EXIT_FUNCTION_TRACE();
    }

    Engine* Engine::Instance() {
        if (singleton == nullptr) {
            singleton = ynew Engine;
        }

        return singleton;
    }

    bool Engine::CmndLine(int argc , char* argv[]) {
        ENTER_FUNCTION_TRACE();

        if (!cmnd_line_handler.Parse(argc , argv)) {
            ENGINE_ERROR("Failed to parse command line arguments");
            return false;
        }

        EXIT_FUNCTION_TRACE();
        return true;
    }

    void Engine::RegisterApplication(App* app) {
        ENTER_FUNCTION_TRACE();

        this->app = app;
        if (app == nullptr) {
            ENGINE_ERROR("Application is null");
            return;
        }

        ENGINE_DEBUG("Registering application: {}", app->ProjectName());
        project_manager = ProjectManager::Instance();
        app_config = project_manager->LoadProject(app , cmnd_line_handler);
        if (!app_config.config_is_valid) {
            ENGINE_ERROR("Failed to load project");
            return;
        }

        if (!ValidateConfiguration()) {
            ENGINE_ERROR("Failed to validate configuration | Paths may be invalid");
            return;
        }

        Filesystem::Initialize(app_config);
 
        app_loaded = true;
        // scene_manager->LoadSceneGraph(project_scene_graph);

        EXIT_FUNCTION_TRACE();
    }

    void Engine::Initialize() {
        ENTER_FUNCTION_TRACE();

        app->PreInitialize();
        
        InitializeSubSytems();

        if (!app->Initialize()) {
            ENGINE_ERROR("Failed to initialize application");
            return;
        }
        running = true;

        EXIT_FUNCTION_TRACE();
    }
    
    void Engine::Run() {
        ENTER_FUNCTION_TRACE();

        delta_time.Start();
        Mouse::SnapToCenter();

        task_manager->FlushTasks();
        while (running) {
            float dt = frame_rate.TimeStep();
            stats->last_frame_time = delta_time.Get();
            task_manager->DispatchTask([stats = stats](){
                for (uint32_t i = 0; i < kFrameTimeBufferSize - 1; ++i)
                    stats->frame_times[i] = stats->frame_times[i + 1];
                stats->frame_times[kFrameTimeBufferSize - 1] = stats->last_frame_time;
            });

            Update(dt);
            task_manager->FlushTasks();

            renderer->Render();
            event_manager->FlushEvents();
            
            frame_rate.Enforce();
        }

        EXIT_FUNCTION_TRACE();
    }

    void Engine::Shutdown() {
        ENTER_FUNCTION_TRACE();

        if (app_loaded) {
            project_manager->Cleanup();
            task_manager->FlushTasks();

            app->Shutdown();

            Systems::Teardown();

            scene_manager->Cleanup();

            resource_handler->Offload();

            renderer->CloseWindow();
            script_engine->Shutdown();
            physics_engine->Cleanup();

            resource_handler->Cleanup();
            event_manager->Cleanup();
            task_manager->Cleanup();
            renderer->Cleanup();
            script_engine->Cleanup();
        }
        
        if (app != nullptr) ydelete app;
        
        ENGINE_INFO("Goodbye");
        EXIT_FUNCTION_TRACE();
        if (singleton != nullptr) ydelete singleton;
    }

}
