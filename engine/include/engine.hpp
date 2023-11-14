#ifndef YE_ENGINE_HPP
#define YE_ENGINE_HPP

#include <iostream>
#include <memory>
#include <queue>
#include <filesystem>

#include "log.hpp"
#include "core/app.hpp"
#include "core/timer.hpp"
#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "core/command_line_args.hpp"
namespace YE {

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
        static Engine* singleton;
        
        EngineStats* stats = nullptr;

        Logger* logger = nullptr;
        ScriptEngine* script_engine = nullptr;
        PhysicsEngine* physics_engine = nullptr;
        Renderer* renderer = nullptr;
        TaskManager* task_manager = nullptr;
        EventManager* event_manager = nullptr;
        ResourceHandler* resource_handler = nullptr;
        SceneManager* scene_manager = nullptr;

        CmndLineHandler cmnd_line_handler;
        std::string program_files;

        time::DeltaTime delta_time;
        time::FrameRateEnforcer<kTargetFps> frame_rate;

        App* app = nullptr;
        EngineConfig app_config;
        std::string project_file_src;

        UUID32 root_window_id{ 0 };

        bool app_loaded = false;
        bool running = false;

        bool FindCoreDirectories();
        bool ValidateConfiguration();
        void InitializeSubSytems();
        void Update(float dt);
        void HandleShutdownEvent();

        Engine();
        ~Engine();
        
        Engine(Engine&&) = delete;
        Engine(const Engine&) = delete;
        Engine& operator=(Engine&&) = delete;
        Engine& operator=(const Engine&) = delete;

        public:

            static Engine* Instance();

            bool CmndLine(int argc , char* argv[]);
            void RegisterApplication(App* app);
            void Initialize();
            void Run();
            void Shutdown();

            inline EngineStats* GetStats() const { return stats; }
            inline float TargetTimeStep() const { return frame_rate.TimeStep(); }
            inline const bool AppLoaded() const { return app_loaded; }
            inline CmndLineHandler& CmndLineHandler() { return cmnd_line_handler; }
    };

}
    
extern YE::App* CreateApp();

#endif // !YE_ENGINE_HPP