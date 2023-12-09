#include "engine.hpp"

#include <string>
#include <queue>
#include <thread>
#include <filesystem>

#include "core/filesystem.hpp"
#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "core/memory/memory_manager.hpp"
#include "event/event_manager.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "reflection/type_name.hpp"
#include "reflection/type_utils.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene_manager.hpp"
#include "physics/physics_engine.hpp"
#include "scripting/script_engine.hpp"
#include "reflection/type_structs.hpp"
#include "reflection/type_description.hpp"

namespace EngineY {

/** Reflection is semi-operatioal, but doesnt actually do anything yet except retriev
 *  the names of the members of a class and sometimes the values of those members
    struct Test {
       int a = 5;
       float b = 3.2f;
       char c = 'c';
       int* d = nullptr;
    
       void Func() {}
       bool Bfunc() { return true; }
    };
    
    REFLECT(
        Test,
        &Test::a, &Test::b, &Test::c, &Test::d,
        &Test::Func, &Test::Bfunc
    );
 */

    bool Engine::ValidateConfiguration() {
        if (!Filesystem::FileExists(app_config.project_path)) {
            ENGINE_ERROR("Project directory does not exist");
            return false;
        }

       if (!Filesystem::FileExists(app_config.project_file)) {
            ENGINE_ERROR("Project file does not exist");
            return false;
        }

        if (!Filesystem::FileExists(app_config.modules_path)) {
            ENGINE_ERROR("Modules directory does not exist");
            return false;
        }

        return true;
    }

    void Engine::HandleShutdownEvent() {
        running = false;
    }

    void Engine::RegisterApplication() {
        this->app = create_app_func();
        if (app == nullptr) {
            ENGINE_ERROR("Application is null");
            return;
        }

        ENGINE_FDEBUG("Registering application: {}", app->ProjectName());
        ENGINE_FINFO("Project Dir :: {}" , app_config.project_path);
        Filesystem::Initialize(app_config);

        app_registered = true;

        // scene_manager->LoadSceneGraph(project_scene_graph);
    }

    void Engine::Initialize() {
        app->PreInitialize();
        
        task_manager = TaskManager::Instance();
        resource_handler = ResourceHandler::Instance();
        event_manager = EventManager::Instance();
        script_engine = ScriptEngine::Instance();
        physics_engine = PhysicsEngine::Instance();
        scene_manager = SceneManager::Instance();

        event_manager->RegisterShutdownCallback([&](ShutdownEvent*) -> bool { 
            this->HandleShutdownEvent();
            return true;
        });

        task_manager->DispatchTask([](){ Keyboard::Initialize(); });
        task_manager->DispatchTask([](){ Mouse::Initialize(); });
       
        ///> This following sequence is horrible because the window needs its framebuffer/framebuffer shader
        ///     so the renderer needs to initialize first, but the resource handler needs a GL context
        ///     to compile shaders
        ///  This can be solved with lazy loading and then we can initialize the resource handler first 
        ///     and then compile the shader when requested
        renderer = Renderer::Instance();
        renderer->Initialize(app , app_config.window_config);

        resource_handler->Load();

        script_engine->Initialize();
        initialized = true;
    }
    
    void Engine::Run() {
        script_engine->LoadProjectModules();
        Systems::Initialize();
        
        if (!app->Initialize()) {
            ENGINE_ERROR("Failed to initialize application");
            return;
        }
 
       
        app->Start();

        running = true;
        while (running) {
            float dt = delta_time.Get();

            // flush events from last frame and poll new ones
            event_manager->FlushEvents();
            event_manager->PollEvents();

            Keyboard::Update();
            Mouse::Update();

            app->Update(dt);
            
            // flush tasks dispatched while updating the application
            task_manager->FlushTasks();

            renderer->Render();
        }

        // cleanup the last frame
        event_manager->FlushEvents();
        task_manager->FlushTasks();

        app->Stop();

        if (app_registered) {
            task_manager->FlushTasks();
            app->Shutdown();
        }
        
        Systems::Teardown();
        
        script_engine->Shutdown();
        resource_handler->Offload();
        renderer->CloseWindow();
        
    }

    void Engine::Shutdown() {
        scene_manager = nullptr;
        physics_engine = nullptr;
        script_engine = nullptr;
        event_manager = nullptr;
        resource_handler = nullptr;
        task_manager = nullptr;
        MemoryManager::Instance()->DeallocateCoreSystems();

        if (app != nullptr) {
            delete app;
        }
        
        ENGINE_INFO("Goodbye");
    }

    void Engine::CoreInitialize(CreateAppFunc func) {
        MemoryManager::Instance()->AllocateCoreSystems();
        logger = Logger::Instance();
        task_manager = TaskManager::Instance();
        resource_handler = ResourceHandler::Instance();
        event_manager = EventManager::Instance();
        script_engine = ScriptEngine::Instance();
        physics_engine = PhysicsEngine::Instance();
        scene_manager = SceneManager::Instance();

        create_app_func = func;
    }

    template <bool keep_qualifiers , typename T>
    void type_name_func_test() {
        std::cout << __FUNCSIG__ << std::endl;
    }

    uint32_t Engine::Main(int argc , char* argv[]) {
        CmndLineHandler cmnd_line_handler(argc , argv);
        if (!cmnd_line_handler.ArgsValid()) {
            ENGINE_CRITICAL("Invalid command line arguments");
            return 1;
        }

        cmnd_line_handler.DumpArgs();
        ConfigBuilder builder(&cmnd_line_handler);
        app_config = builder.Build();

        ENGINE_FINFO("CWD :: [{}]" , Filesystem::GetCWD());
        ENGINE_FINFO("Project name :: [{}]" , app_config.project_name);
        ENGINE_FINFO("Project file :: [{}]" , app_config.project_file);
        ENGINE_FINFO("Project path :: [{}]" , app_config.project_path);
        ENGINE_FINFO("Modules path :: [{}]" , app_config.modules_path);
        if (!ValidateConfiguration()) {
            ENGINE_ERROR("Failed to validate configuration | Paths may be invalid");
            return 1;
        }
        
        RegisterApplication();

        if (!app_registered) {
            ENGINE_ERROR("Failed to load application");
            return 1;
        }

        Initialize();

        if (!initialized) {
            ENGINE_ERROR("Engine failed to initialized!");
            return 1;
        } else {
            delta_time.Start();
            Mouse::SnapToCenter();

            task_manager->FlushTasks();
        }

        Run();
        Shutdown();

        return 0;
    }

}
