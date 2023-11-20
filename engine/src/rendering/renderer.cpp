#include "rendering/renderer.hpp"

#include <cstdio>

#include <SDL.h>
#include <glad/glad.h>

#include "core/defines.hpp"
#include "core/log.hpp"
#include "application/app.hpp"
#include "core/hash.hpp"
#include "event/event_manager.hpp"
#include "scene/scene.hpp"
#include "scene/components.hpp"
#include "rendering/gui.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/camera.hpp"
#include "rendering/framebuffer.hpp"

namespace EngineY {

    Renderer* Renderer::singleton = nullptr;

    bool Renderer::CheckID(UUID32 id , const std::string& name , const RenderCallbackMap& map) {
        if (map.find(id) != map.end()) {
            ENGINE_WARN("Failed to register pre-execute callback :: [{0}] | Name already exists" , name);
            return false;
        }
        return true;
    }
    
    void Renderer::SetSDLWindowAttributes(WindowConfig& config) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK , SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION , kOpenGLMajorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION , kOpenGLMinorVersion);       
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER , kOpenGLDoubleBuffer);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE , config.gl_stencil_size);
        SDL_GL_SetAttribute(
            SDL_GL_RED_SIZE , 
            config.gl_color_bits.r > 8 ? 
                8 : config.gl_color_bits.r
        );
        SDL_GL_SetAttribute(
            SDL_GL_GREEN_SIZE , 
            config.gl_color_bits.g > 8 ?
                8 : config.gl_color_bits.g
        );
        SDL_GL_SetAttribute(
            SDL_GL_BLUE_SIZE , 
            config.gl_color_bits.b > 8 ? 
                8 : config.gl_color_bits.b
        );
        SDL_GL_SetAttribute(
            SDL_GL_ALPHA_SIZE , 
            config.gl_color_bits.a > 8 ? 
                8 : config.gl_color_bits.a
        );
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS , config.gl_multisample_buffers);
        SDL_GL_SetAttribute(
            SDL_GL_ACCELERATED_VISUAL , 
            config.accelerated_visual ? 1 : 0
        );

        SDL_GL_SetAttribute(
            SDL_GL_MULTISAMPLESAMPLES , 
            config.gl_multisample_samples > 16 ?
                16 : config.gl_multisample_samples
        );
    }
    
    void Renderer::EnableGLSettings(WindowConfig& config) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        glClearColor(config.clear_color.r , config.clear_color.g , config.clear_color.b , config.clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void Renderer::PopCamera() {
        if (render_camera == nullptr) {
            return;
        }
        render_camera = nullptr;
    }
    
    Renderer* Renderer::Instance() {
        if (singleton == nullptr) {
            singleton = new Renderer;
        }
        return singleton;
    }

    void Renderer::SubmitRenderCmnd(std::unique_ptr<RenderCommand>& cmnd) {
        commands.push(std::move(cmnd));
    }

    void Renderer::SubmitDebugRenderCmnd(std::unique_ptr<RenderCommand>& cmnd) {
        debug_commands.push(std::move(cmnd));
    }
    
    void Renderer::RegisterSceneContext(Scene* scene) {
        if (scene == nullptr) {
            ENGINE_WARN("Failed to register scene context | Scene is nullptr");
            return;
        }
        SetSceneRenderMode(scene->current_render_mode);
    }

    void Renderer::PushFramebuffer(const std::string& name , Framebuffer* framebuffer) {
        if (framebuffer == nullptr) {
            ENGINE_WARN("Failed to push framebuffer | Framebuffer is nullptr");
            return;
        }
        UUID32 id = Hash::FNV32(name);
        if (framebuffers.find(id) != framebuffers.end()) {
            ENGINE_WARN("Failed to push framebuffer | Name [{0}] already exists" , name);
            return;
        }
        framebuffers[id] = framebuffer;
    }
    
    void Renderer::ActivateFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        ActivateFramebuffer(id);
    }
    
    void Renderer::ActivateFramebuffer(UUID32 id) {
        if (framebuffers.find(id) == framebuffers.end()) {
            ENGINE_WARN("Failed to activate framebuffer | ID [{0}] does not exist" , id.uuid);
            return;
        }
        active_framebuffer = id;
        framebuffer_active = true;
    }
    
    void Renderer::SetDefaultFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        SetDefaultFramebuffer(id);
    }
    
    void Renderer::SetDefaultFramebuffer(UUID32 id) {
        if (framebuffers.find(id) == framebuffers.end()) {
            ENGINE_WARN("Failed to set default framebuffer | ID [{0}] does not exist" , id.uuid);
            return;
        }
        default_framebuffer = id;
    }
   
    void Renderer::RevertToDefaultFramebuffer() {
        active_framebuffer = default_framebuffer;
    }
    
    void Renderer::ActivateLastFramebuffer() {
        active_framebuffer = last_active_framebuffer;
    }
    
    void Renderer::DeactivateFramebuffer() {
        last_active_framebuffer = active_framebuffer;
        active_framebuffer = default_framebuffer;
        if (default_framebuffer.uuid == 0) {
            framebuffer_active = false;
        }
    }
    
    void Renderer::PopFramebuffer(const std::string& name ) {
        UUID32 id = Hash::FNV32(name);
        PopFramebuffer(id);
    }
    
    void Renderer::PopFramebuffer(UUID32 id) {
        if (framebuffers.find(id) == framebuffers.end()) {
            ENGINE_WARN("Failed to pop framebuffer | ID [{0}] does not exist" , id.uuid);
            return;
        }
        framebuffers.erase(id);
    }
    
    Framebuffer* Renderer::GetWindowFramebuffer() {
        return window->GetFramebuffer();
    }
    
    Framebuffer* Renderer::GetActiveFramebuffer() {
        if (framebuffers.find(active_framebuffer) == framebuffers.end()) {
            ENGINE_WARN("Failed to get active framebuffer | ID [{0}] does not exist" , active_framebuffer.uuid);
            return nullptr;
        }
        return framebuffers[active_framebuffer];
    }

    Framebuffer* Renderer::GetFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        return GetFramebuffer(id);
    }
    
    Framebuffer* Renderer::GetFramebuffer(UUID32 id) {
        if (framebuffers.find(id) == framebuffers.end()) {
            ENGINE_WARN("Failed to get framebuffer | ID [{0}] does not exist" , id.uuid);
            return nullptr;
        }
        return framebuffers[id];
    }

    void Renderer::PushCamera(Camera* camera) {
        if (camera == nullptr) {
            ENGINE_WARN("Failed to push camera | Camera is nullptr");
            return;
        }
        render_camera = camera;
    }    

    void Renderer::SetSceneRenderMode(RenderMode mode) {
        scene_render_mode = mode;
    }

    void Renderer::Initialize(App* app , WindowConfig& config) {
        app_handle = app;
        
        config.flags |= SDL_WINDOW_OPENGL;
        if (config.fullscreen) config.flags |= SDL_WINDOW_FULLSCREEN;

        window = ynew(Window);
        
        int sdl_init = SDL_InitSubSystem(SDL_INIT_VIDEO);
        ENGINE_ASSERT(
            sdl_init == 0 , 
            "Failed to initialize SDL2 {0}" ,
            SDL_GetError()
        );
        
        SetSDLWindowAttributes(config);

        window->Open(config);
        
        int glad_init = gladLoadGLLoader(SDL_GL_GetProcAddress);
        ENGINE_ASSERT(glad_init != 0 , "GLAD failed to initialize");

        EnableGLSettings(config);

        // we should be able to get rid of this call if we handle the framebuffer correctly
        window->InitializeFramebuffer();
        window->RegisterCallbacks();
        
        gui = ynew(Gui);
        gui->Initialize(window);
        
        window->Clear();
        window->SwapBuffers();
    }

    void Renderer::Render() {
        window->Clear();

        glPolygonMode(GL_FRONT_AND_BACK , scene_render_mode);

        ///> we need to fix this so that we clear each framebuffer, 
        ///     then render to each framebuffer, then draw each framebuffer
        ///     but the problem is that only one can be bound at a time
        ///     so either we just let there be one framebuffer or get
        ///     super creative with how we handle this
        for (auto& [id , fb] : framebuffers) {
            fb->BindFrame();
        }

        app_handle->Draw();
        
        for (auto& [id , renderable] : persistent_renderables) {
            renderable->Execute(render_camera , ShaderUniforms{});
        }

        while (!commands.empty()) {
            commands.front()->Execute(render_camera , ShaderUniforms{});
            commands.pop();
        }

        glPolygonMode(GL_FRONT_AND_BACK , RenderMode::LINE);

        for (auto& [id , renderable] : debug_renderables)
            renderable->Execute(render_camera , ShaderUniforms{});
        
        while (!debug_commands.empty()) {
            debug_commands.front()->Execute(render_camera , ShaderUniforms{});
            debug_commands.pop();
        }

        PopCamera();

        glPolygonMode(GL_FRONT_AND_BACK , RenderMode::FILL);

        for (auto& [id , fb] : framebuffers) {
            fb->UnbindFrame();
        }

        window->Draw();

        if (framebuffer_active && active_framebuffer.uuid != 0) {
            framebuffers[active_framebuffer]->Draw();
        }

        gui->BeginRender(window->GetSDLWindow());
        app_handle->DrawGui();
        gui->EndRender(window->GetSDLWindow() , window->GetGLContext());

        window->SwapBuffers();
    }
    
    void Renderer::CloseWindow() {
        gui->Shutdown();
        window->Close();
    }
    
    void Renderer::Cleanup() {
        for (auto& [id , renderable] : persistent_renderables) {
            ydelete(renderable);
        }
        persistent_renderables.clear();

        for (auto& [id , renderable] : debug_renderables) {
            ydelete(renderable);
        }
        debug_renderables.clear();

        for (auto& [id , framebuffer] : framebuffers) {
            framebuffer->Destroy();
            ydelete(framebuffer);
        }

        ENGINE_ASSERT(gui != nullptr , "Attempting to cleanup renderer without initializing it");
        ydelete(gui);

        ENGINE_ASSERT(window != nullptr , "Attempted to cleanup renderer without initializing it");
        ydelete(window);
    }

}
