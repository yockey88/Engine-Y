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
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene.hpp"
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
    
    std::filesystem::path Engine::FindProjectFile() {
        ENTER_FUNCTION_TRACE();
        YE_CRITICAL_ASSERTION(app != nullptr , "Application is null");

        std::string project_name = app->ProjectName();
        std::string file_name = project_name;

        std::vector<std::string> extensions = { "yproj" , ".ysc" , ".ys" , ".y" };
        std::filesystem::path path = project_name;
        path /= file_name;
        for (uint32_t i = 0; i < extensions.size(); ++i) {
            path.replace_extension(extensions[i]);
            if (std::filesystem::exists(path)) {
                EXIT_FUNCTION_TRACE();
                return path;
            }
        }

        EXIT_FUNCTION_TRACE();
        return std::filesystem::path();
    }

    bool Engine::FindCoreDirectories() {
        ENTER_FUNCTION_TRACE();

        if (!cmnd_line_handler.FlagExists(CmndLineFlag::WORKING_DIR)) {
            app_config.working_directory = std::filesystem::current_path().string();
        } else {
            app_config.working_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::WORKING_DIR);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_PATH)) {
            if (std::filesystem::current_path().string() == app_config.project_name) {
                app_config.project_path = std::filesystem::current_path().string();
            } else if (std::filesystem::exists(app_config.working_directory + "/" + app_config.project_name)) {
                app_config.project_path = app_config.working_directory + "/" + app_config.project_name;
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / app_config.project_name
            )) {
                app_config.project_path = program_files + "/EngineY/" + app_config.project_name;
            } else {
                ENGINE_ERROR("Failed to find project directory");
                return false;
            }
        } else {
            app_config.project_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_PATH);
        }

        if (!cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_FILE)) {
            if (std::filesystem::exists(
                std::filesystem::path(app_config.working_directory) / app_config.project_name / (app_config.project_name + ".yproj")
            )) {
                app_config.project_file = app_config.working_directory + "/" + app_config.project_name + "/" + app_config.project_name + ".yproj";
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / app_config.project_name / (app_config.project_name + ".yproj")
            )) {
                app_config.project_file = program_files + "/EngineY/" + app_config.project_name + "/" + app_config.project_name + ".yproj";
            } else if (std::filesystem::exists(
                std::filesystem::path(app_config.project_path) / (app_config.project_name + ".yproj")
            )) {
                app_config.project_file = (std::filesystem::path(app_config.project_path) / (app_config.project_name + ".yproj")).string();
            } else {
                ENGINE_ERROR("Failed to find project file");
                return false;
            }
        } else {
            app_config.project_file = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_FILE);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::MODULES_DIR)) {
            if (std::filesystem::exists(std::filesystem::path(app_config.project_path) / "modules")) {
                app_config.modules_directory = (std::filesystem::path(app_config.project_path) / "modules").string();
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / "bin" / "Debug" / "modules"
            )) {
                app_config.modules_directory = program_files + "/EngineY/bin/Debug/modules";
            } else {
                ENGINE_ERROR("Failed to find modules directory");
                return false;
            }
        } else {
            app_config.modules_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::MODULES_DIR);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::MONO_CONFIG_PATH)) {
            app_config.mono_config_path = app_config.mono_dll_path;
        } else {
            app_config.mono_config_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::MONO_CONFIG_PATH);
        } 

        EXIT_FUNCTION_TRACE();
    }
    
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

        if (!Filesystem::FileExists(app_config.mono_dll_path)) {
            ENGINE_ERROR("Mono dll directory does not exist");
            return false;

        }

        EXIT_FUNCTION_TRACE();
    }

    void Engine::InitializeSubSytems() { 
        ENTER_FUNCTION_TRACE();

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
        ENTER_FUNCTION_TRACE_MSG(app->ProjectName());

        this->app = app;
        if (app == nullptr) {
            ENGINE_ERROR("Application is null");
            return;
        }

#ifdef YE_PLATFORM_WIN
        try {
            program_files = std::getenv("ProgramFiles");
        } catch (const std::exception& e) {
            ENGINE_ERROR("Failed to retrieve ProgramFiles environment variable: {}", e.what());
            return;
        }
#else
        YE_CRITICAL_ASSERTION(false , "UNIMPLEMENTED");
#endif

        app_config = app->GetEngineConfig();
        app_config.engine_root = std::filesystem::current_path().string(); // program_files + "/EngineY";
        app_config.mono_dll_path = app_config.engine_root + "/external";

        if (app->ProjectName() == "editor") 
            logger->OpenConsole();

        if (cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_NAME)) 
            app_config.project_name = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_NAME);
        else 
            app_config.project_name = app->ProjectName();
        
        if (cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_FILE)) { 
            app_config.use_project_file = true;
            app_config.project_file = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_FILE);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::WORKING_DIR) || !cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_PATH)   ||
            !cmnd_line_handler.FlagExists(CmndLineFlag::MODULES_DIR)  || !cmnd_line_handler.FlagExists(CmndLineFlag::MONO_CONFIG_PATH)) {
            if (!FindCoreDirectories()) {
                ENGINE_ERROR("Failed to find core directories");
                return;
            }
        } else {
            if (cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_PATH)) 
                app_config.project_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_PATH);

            if (cmnd_line_handler.FlagExists(CmndLineFlag::WORKING_DIR)) 
                app_config.working_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::WORKING_DIR);

            if (cmnd_line_handler.FlagExists(CmndLineFlag::MODULES_DIR))
                app_config.modules_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::MODULES_DIR);
        
            if (cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_BIN))
                app_config.project_bin = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_BIN);
        
            if (cmnd_line_handler.FlagExists(CmndLineFlag::MONO_CONFIG_PATH))
                app_config.mono_config_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::MONO_CONFIG_PATH);
        
            if (cmnd_line_handler.FlagExists(CmndLineFlag::ENGINE_ROOT))
                app_config.engine_root = cmnd_line_handler.RetrieveValue(CmndLineFlag::ENGINE_ROOT);
        }

        if (!ValidateConfiguration()) {
            ENGINE_ERROR("Failed to validate configuration | Paths may be invalid");
            return;
        }

        Filesystem::Initialize(app_config);
        
        if (app_config.use_project_file) {
            std::filesystem::path project_path = app_config.project_file;
            if (project_path.empty()) {
                ENGINE_ERROR("Project file missing");
                return;
            }

            YE::YScriptLexer lexer(project_path.string());

            auto [src , tokens] = lexer.Lex();
            project_file_src = src;

            YE::YScriptParser parser(tokens);
            ProjectAst parse_tree = parser.Parse();

            YS::Interpreter interpreter(parse_tree);
            project_scene_graph = interpreter.BuildScene();
            YS::ProjectMetadata metadata = interpreter.ProjectMetadata();
        } 
        app_loaded = true;
        
        task_manager = TaskManager::Instance();
        resource_handler = ResourceHandler::Instance();
        event_manager = EventManager::Instance();
        renderer = Renderer::Instance(); 
        script_engine = ScriptEngine::Instance();
        physics_engine = PhysicsEngine::Instance();

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
        }
        
        LOG_INFO("Goodbye");
        EXIT_FUNCTION_TRACE();
        if (singleton != nullptr) ydelete singleton;
    }

}