#ifndef ENGINEY_ENGINE_HPP
#define ENGINEY_ENGINE_HPP

#include <iostream>
#include <memory>
#include <queue>
#include <filesystem>

#include "core/log.hpp"
#include "application/app.hpp"
#include "application/application_config.hpp"
#include "core/timer.hpp"
#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "core/command_line_args.hpp"

namespace EngineY {

    class Logger;
    class ScriptEngine;
    class PhysicsEngine;
    class Renderer;
    class TaskManager;
    class EventManager;
    class ResourceHandler;
    class SceneManager;

    static constexpr uint32_t kFrameTimeBufferSize = 1000;

    struct EngineStats {
        float last_frame_time = 0.0f;
        float frame_times[kFrameTimeBufferSize];
    };

    class Engine {
        
        EngineStats* stats = nullptr;

        Logger* logger = nullptr;
        ScriptEngine* script_engine = nullptr;
        PhysicsEngine* physics_engine = nullptr;
        TaskManager* task_manager = nullptr;
        EventManager* event_manager = nullptr;
        ResourceHandler* resource_handler = nullptr;
        SceneManager* scene_manager = nullptr;

        Renderer* renderer = nullptr;
        time::FrameRateEnforcer<kTargetFps> frame_rate;
        time::DeltaTime delta_time;

        CreateAppFunc create_app_func = nullptr;
        App* app = nullptr;
        ApplicationConfig app_config;
        std::string project_file_src;


        std::condition_variable render_ready_cv;
        bool render_ready = false;

        UUID32 root_window_id{ 0 };

        bool app_registered = false;
        bool initialized = false;

        bool app_loaded = false;
        bool running = false;

        bool ValidateConfiguration();
        void HandleShutdownEvent();
        
        void RegisterApplication();
        void Initialize();
        void Run();
        void Shutdown();

        public:
            Engine() {}
            ~Engine() {}

            void CoreInitialize(CreateAppFunc func);
            uint32_t Main(int argc , char* argv[]);

            inline EngineStats* GetStats() const { return stats; }
            inline float TargetTimeStep() const { return frame_rate.TimeStep(); }
            inline const bool AppLoaded() const { return app_loaded; }
    };

}
    
extern  EngineY::App* CreateApp();

#endif // !YE_ENGINE_HPP
