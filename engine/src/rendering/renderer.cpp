#include "rendering/renderer.hpp"

#include <cstdio>

#include <SDL.h>
#include <glad/glad.h>

#include "log.hpp"
#include "engine.hpp"
#include "core/app.hpp"
#include "core/hash.hpp"
#include "event/event_manager.hpp"
#include "scene/scene.hpp"
#include "scene/components.hpp"
#include "rendering/gui.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/camera.hpp"
#include "rendering/framebuffer.hpp"

namespace YE {

    Renderer* Renderer::singleton = nullptr;

    bool Renderer::CheckID(UUID32 id , const std::string& name , const RenderCallbackMap& map) {
        if (map.find(id) != map.end()) {
            YE_WARN("Failed to register pre-execute callback :: [{0}] | Name already exists" , name);
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
    
    void Renderer::RegisterCallbacks() {
        EventManager::Instance()->RegisterWindowResizedCallback(
            [fbs = &framebuffers](WindowResized* event) -> bool {
                if (fbs->size() == 0) return true; // no framebuffers to resize

                UUID32 id = Renderer::Instance()->ActiveFramebuffer();
                if (fbs->find(id) != fbs->end()) {
                    (*fbs)[id]->HandleResize({ event->Width() , event->Height() });
                } else {
                    YE_WARN("Failed to resize main framebuffer :: [{0}] | Framebuffer does not exist" , id.uuid);
                    return false;
                }
                return true;
            } ,
            "active-framebuffer-resize"
        );
    }
    
    void Renderer::BeginRender() {
        window->Clear();

        glPolygonMode(GL_FRONT_AND_BACK , scene_render_mode);

        if (framebuffer_active)
            framebuffers[active_framebuffer]->BindFrame();
        
        app_handle->Draw();
    }
    
    void Renderer::Execute() {
        for (auto& [id , renderable] : persistent_renderables)
            renderable->Execute(render_camera , ShaderUniforms{});

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

        render_camera = nullptr;

        glPolygonMode(GL_FRONT_AND_BACK , RenderMode::FILL);

        if (framebuffer_active)
            framebuffers[active_framebuffer]->UnbindFrame();
    }
    
    void Renderer::EndRender() {
        if (framebuffer_active)
            framebuffers[active_framebuffer]->Draw();

        gui->BeginRender(window->GetSDLWindow());
        app_handle->DrawGui();
        gui->EndRender(window->GetSDLWindow() , window->GetGLContext());

        window->SwapBuffers();
    }

    Renderer* Renderer::Instance() {
        if (singleton == nullptr) {
            singleton = ynew Renderer;
        }
        return singleton;
    }

    void Renderer::RegisterPreRenderCallback(std::function<void()> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID(id , name , PreRenderCallbacks)) return;
        PreRenderCallbacks[id] = callback;
    }

    void Renderer::RegisterPostRenderCallback(std::function<void()> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID(id , name , PostRenderCallbacks)) return;
        PostRenderCallbacks[id] = callback;
    }

    void Renderer::Initialize(App* app) {
        app_handle = app;

        WindowConfig win_config = app->GetWindowConfig();
        
        win_config.flags |= SDL_WINDOW_OPENGL;
        if (win_config.fullscreen) win_config.flags |= SDL_WINDOW_FULLSCREEN;

        window = ynew Window;
        gui = ynew Gui;

        int sdl_init = SDL_InitSubSystem(SDL_INIT_EVENTS);
        YE_CRITICAL_ASSERTION(
            sdl_init == 0 , 
            "Failed to initialize SDL2 {0}" ,
            SDL_GetError()
        );
        
        SetSDLWindowAttributes(win_config);
        window->Open(win_config); 

        int glad_init = gladLoadGLLoader(SDL_GL_GetProcAddress);
        YE_CRITICAL_ASSERTION(glad_init != 0 , "GLAD failed to initialize");

        EnableGLSettings(win_config);

        window->RegisterCallbacks();
        RegisterCallbacks();
        
        gui->Initialize(window);
        
        window->Clear();
        window->SwapBuffers();
    }

    void Renderer::PushFramebuffer(const std::string& name , Framebuffer* framebuffer) {
        if (framebuffer == nullptr) {
            YE_WARN("Failed to push framebuffer :: [{0}] | Framebuffer is nullptr" , name);
            return;
        }

        UUID32 id = Hash::FNV32(name);
        if (framebuffers.find(id) != framebuffers.end()) {
            YE_WARN("Failed to push framebuffer :: [{0}] | Name already exists" , name);
            return;
        }

        framebuffers[id] = framebuffer;
    }

    void Renderer::SubmitRenderCmnd(std::unique_ptr<RenderCommand>& cmnd) {
        commands.push(std::move(cmnd));
    }

    void Renderer::SubmitDebugRenderCmnd(std::unique_ptr<RenderCommand>& cmnd) {
        debug_commands.push(std::move(cmnd));
    }

    void Renderer::PopFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (framebuffers.find(id) == framebuffers.end()) {
            YE_WARN("Failed to pop framebuffer :: [{0}] | Did you push it to the renderer?" , name);
            return;
        }

        Framebuffer* framebuffer = framebuffers[id];
        framebuffers.erase(id);
        ydelete framebuffer;

        id = 0;
    }

    void Renderer::SetDefaultFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (framebuffers.find(id) == framebuffers.end()) {
            YE_WARN("Failed to set default framebuffer :: [{0}] | Did you push it to the renderer?" , name);
            return;
        }
        default_framebuffer = id;
    }

    void Renderer::RevertToDefaultFramebuffer() {
        if (framebuffers.find(default_framebuffer) == framebuffers.end()) {
            YE_WARN("Failed to revert to default framebuffer | Did you set a default?");
            return;
        }
        active_framebuffer = default_framebuffer;
        framebuffer_active = true;
    }

    void Renderer::ActivateFramebuffer(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (framebuffers.find(id) == framebuffers.end()) {
            YE_WARN("Failed to activate framebuffer :: [{0}] | Did you push it to the renderer?" , name);
            return;
        }
        
        active_framebuffer = id;
        last_active_framebuffer = id;
        framebuffer_active = true;
    }
        
    void Renderer::ActivateLastFramebuffer() {
        if (framebuffers.find(last_active_framebuffer) == framebuffers.end()) {
            YE_WARN("Failed to activate last framebuffer | Attempting to revert to default");
            RevertToDefaultFramebuffer();
        }
        active_framebuffer = last_active_framebuffer;
        framebuffer_active = true;
    }
    
    void Renderer::DeactivateFramebuffer() {
        framebuffer_active = false;
        active_framebuffer = 0;
    }
    
    void Renderer::RegisterSceneContext(Scene* scene) {
        if (scene == nullptr) {
            YE_WARN("Failed to register scene context | Scene is nullptr");
            return;
        }
        SetSceneRenderMode(scene->current_render_mode);
    }

    void Renderer::PushCamera(Camera* camera) {
        if (camera == nullptr) {
            YE_WARN("Failed to push camera | Camera is nullptr");
            return;
        }
        render_camera = camera;
    }

    void Renderer::SetSceneRenderMode(RenderMode mode) {
        scene_render_mode = mode;
    }

    void Renderer::Render() {
        for (auto& [id , cb] : PreRenderCallbacks)
            cb();
        BeginRender();
        Execute();
        EndRender();
        for (auto& [id , cb] : PostRenderCallbacks)
            cb();
    }
    
    void Renderer::CloseWindow() {
        gui->Shutdown();
        window->Close();
    }
    
    void Renderer::Cleanup() {
        for (auto& [id , renderable] : persistent_renderables)
            ydelete renderable;
        persistent_renderables.clear();

        for (auto& [id , renderable] : debug_renderables)
            ydelete renderable;
        debug_renderables.clear();

        for (auto& [id , fb] : framebuffers)
            ydelete fb;
        framebuffers.clear();

        if (gui != nullptr) ydelete gui;

        YE_CRITICAL_ASSERTION(window != nullptr , "Attempted to cleanup renderer without initializing it");
        ydelete window;

        if (singleton != nullptr) ydelete singleton;
    }

}