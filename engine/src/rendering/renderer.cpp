#include "rendering/renderer.hpp"

#include <cstdio>

#include <SDL.h>
#include <glad/glad.h>

#include "log.hpp"
#include "engine.hpp"
#include "core/app.hpp"
#include "core/hash.hpp"
#include "core/window.hpp"
#include "core/window_manager.hpp"
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
    
    void Renderer::BeginRender() {
        window_manager->ClearWindows();

        gui->BeginRender(main_window->GetSDLWindow());

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

        gui->Render(main_window);
        app_handle->DrawGui();
        gui->EndRender(main_window->GetSDLWindow() , main_window->GetGLContext());

        window_manager->SwapWindowBuffers();
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
        gui = ynew Gui;

        window_manager = WindowManager::Instance();
        window_manager->Initialize(gui);
    }

    void Renderer::OpenWindow(const WindowConfig& config) {
        main_window = window_manager->OpenWindow(config);
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

    void Renderer::SubmitRenderCmnd(std::unique_ptr<RenderCommand>& cmnd /* , const std::string& target_window */) {
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
    
    void Renderer::CloseWindow(const std::string& name) {
        gui->Shutdown();
        window_manager->CloseWindows();
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

        window_manager->Cleanup();
        if (singleton != nullptr) ydelete singleton;
    }

}