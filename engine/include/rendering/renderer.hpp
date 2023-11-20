#ifndef ENGINEY_RENDERER_HPP
#define ENGINEY_RENDERER_HPP

#include <string>
#include <queue>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>

#include <glad/glad.h>

#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "core/timer.hpp"
#include "rendering/window.hpp"
#include "rendering/render_commands.hpp"

constexpr uint32_t kOpenGLMajorVersion = 4;
constexpr uint32_t kOpenGLMinorVersion = 6;
constexpr uint32_t kOpenGLDoubleBuffer = 1;
constexpr uint32_t kOpenGLSwapInterval = 1;

namespace EngineY {

namespace components {

    struct Renderable;

}

    class App;
    class Gui;
    class VertexArray;
    class Framebuffer;
    class Scene;
    class Camera;
    class Framebuffer;

    using RenderQueue = std::queue<std::unique_ptr<RenderCommand>>;
    using VertexMap = std::unordered_map<UUID32 , VertexArray*>;
    using RenderMap = std::unordered_map<UUID32 , RenderCommand*>;
    using FramebufferMap = std::unordered_map<UUID32 , Framebuffer*>;

    using PostRenderCallback = std::function<void(Framebuffer*)>;

    using RenderCallbackMap = std::unordered_map<UUID32 , PostRenderCallback>;

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

        Window* window = nullptr;
        Gui* gui = nullptr;
        Camera* render_camera = nullptr;

        App* app_handle = nullptr;
        
        RenderQueue commands;
        RenderQueue debug_commands;
        
        // used for persisting render commands
        RenderMap persistent_renderables;
        RenderMap debug_renderables;

        FramebufferMap framebuffers;

        UUID32 default_framebuffer{ 0 };
        UUID32 active_framebuffer{ 0 };
        UUID32 last_active_framebuffer{ 0 };

        RenderMode scene_render_mode = RenderMode::FILL;

        bool debug_rendering = false;
        bool framebuffer_active = false;

        bool CheckID(UUID32 id , const std::string& name , const RenderCallbackMap& map);
        
        void SetSDLWindowAttributes(WindowConfig& config);
        void EnableGLSettings(WindowConfig& config);

        void PopCamera();
        
        Renderer() {}
        ~Renderer() {}
        
        Renderer(Renderer&&) = delete;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        public:
            static Renderer* Instance();

            void PushFramebuffer(const std::string& name , Framebuffer* framebuffer);
            void ActivateFramebuffer(const std::string& name);
            void ActivateFramebuffer(UUID32 id);
            void SetDefaultFramebuffer(const std::string& name);
            void SetDefaultFramebuffer(UUID32 id);
            void RevertToDefaultFramebuffer();
            void ActivateLastFramebuffer();
            void DeactivateFramebuffer();
            void PopFramebuffer(const std::string& name );            
            void PopFramebuffer(UUID32 id);            

            Framebuffer* GetWindowFramebuffer();
            Framebuffer* GetActiveFramebuffer();
            Framebuffer* GetFramebuffer(const std::string& name);
            Framebuffer* GetFramebuffer(UUID32 id);
            
            void PushCamera(Camera* camera);
            
            void SubmitRenderCmnd(std::unique_ptr<RenderCommand>& cmnd);
            void SubmitDebugRenderCmnd(std::unique_ptr<RenderCommand>& cmnd);

            void RegisterSceneContext(Scene* scene);
            
            void SetSceneRenderMode(RenderMode mode);

            void Initialize(App* app , WindowConfig& config);

            void Render();

            void CloseWindow();
            void Cleanup();
            
            inline Window* ActiveWindow() { return window; }
            inline bool DebugRendering() const { return debug_rendering; }
    };

}

#endif
