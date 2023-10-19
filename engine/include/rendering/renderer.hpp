#ifndef YE_RENDERER_HPP
#define YE_RENDERER_HPP

#include <string>
#include <queue>
#include <memory>
#include <unordered_map>
#include <functional>

#include <glad/glad.h>

#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "core/timer.hpp"
#include "rendering/render_commands.hpp"

namespace YE {

namespace components {

    struct Renderable;

}

    struct WindowConfig;

    class App;
    class Window;
    class Gui;
    class VertexArray;
    class Framebuffer;
    class Scene;
    class Camera;

    class WindowManager;

    using RenderQueue = std::queue<std::unique_ptr<RenderCommand>>;
    using VertexMap = std::unordered_map<UUID32 , VertexArray*>;
    using RenderMap = std::unordered_map<UUID32 , RenderCommand*>;
    using FramebufferMap = std::unordered_map<UUID32 , Framebuffer*>;
    using RenderCallbackMap = std::unordered_map<UUID32 , std::function<void()>>;

    enum RenderMode {
        POINT = GL_POINT ,
        LINE = GL_LINE ,
        FILL = GL_FILL
    };

    enum class DrawGroup {
        DEFAULT = 0 ,
        DEBUG = 1
    };

    class Renderer {

        static Renderer* singleton;

        App* app_handle = nullptr;

        WindowManager* window_manager = nullptr;
        Window* main_window = nullptr;
        Gui* gui = nullptr;
        Camera* render_camera = nullptr;
        
        RenderQueue commands;
        RenderQueue debug_commands;
        
        // used for persisting render commands
        RenderMap persistent_renderables;
        RenderMap debug_renderables;

        FramebufferMap framebuffers;
        RenderCallbackMap PreRenderCallbacks;
        RenderCallbackMap PostRenderCallbacks;

        UUID32 default_framebuffer{ 0 };
        UUID32 active_framebuffer{ 0 };
        UUID32 last_active_framebuffer{ 0 };

        RenderMode scene_render_mode = RenderMode::FILL;

        bool debug_rendering = false;
        bool framebuffer_active = false;

        FramebufferMap* FramebufferMap() { return &framebuffers; } // for windows

        bool CheckID(UUID32 id , const std::string& name , const RenderCallbackMap& map);
        
        void BeginRender();
        void Execute();
        void EndRender();

        friend class Window;

        Renderer() {}
        ~Renderer() {}

        Renderer(Renderer&&) = delete;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        public:

            static Renderer* Instance();

            void RegisterPreRenderCallback(std::function<void()> callback , const std::string& name);
            void RegisterPostRenderCallback(std::function<void()> callback , const std::string& name);

            void Initialize(App* app);

            void OpenWindow(const WindowConfig& config);
            
            void PushFramebuffer(const std::string& name , Framebuffer* framebuffer);
            void SubmitRenderCmnd(std::unique_ptr<RenderCommand>& cmnd /* , const std::string& target_window */);
            void SubmitDebugRenderCmnd(std::unique_ptr<RenderCommand>& cmnd);

            /// \todo implement these functions so we can stop submitting render commands every frame that can persist between frames
            // void PushRenderable(RenderCommand* cmnd , const std::string& name , DrawGroup group = DrawGroup::DEFAULT);
            // void UpdateRenderCmnd(RenderCommand* new_cmnd , const std::string& name , DrawGroup group = DrawGroup::DEFAULT);
            // void RemoveRenderCmnd(const std::string& name , DrawGroup group = DrawGroup::DEFAULT);

            void PopFramebuffer(const std::string& name);
            
            void SetDefaultFramebuffer(const std::string& name);
            void RevertToDefaultFramebuffer();

            void ActivateFramebuffer(const std::string& name);
            void ActivateLastFramebuffer();
            void DeactivateFramebuffer();

            void RegisterSceneContext(Scene* scene);
            void PushCamera(Camera* camera);
            
            void SetSceneRenderMode(RenderMode mode);

            void Render();

            void CloseWindow(const std::string& name);
            void Cleanup();
            
            inline Window* ActiveWindow() { return main_window; }
            inline UUID32 ActiveFramebuffer() const { return active_framebuffer; }
            inline bool DebugRendering() const { return debug_rendering; }
            inline bool FramebufferActive() const { return framebuffer_active; }
    };

}

#endif