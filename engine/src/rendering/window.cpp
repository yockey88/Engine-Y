#include "rendering/window.hpp"

#include "core/log.hpp"

#include "core/defines.hpp"
#include "core/resource_handler.hpp"
#include "event/event_manager.hpp"
#include "rendering/shader.hpp"
#include "rendering/framebuffer.hpp"

namespace EngineY {


    void Window::HandleResize(glm::ivec2 size) {
        this->size = size;
    }

    void Window::Open(const WindowConfig& config) {
        clear_color = config.clear_color;
        size = config.size;
        position = config.position;
        min_size = config.min_size;
        fullscreen = config.fullscreen;
        rendering_to_screen = config.rendering_to_screen;

        window = SDL_CreateWindow(
            config.title.c_str() ,
            config.position.x , config.position.y ,
            config.size.x , config.size.y ,
            config.flags
        );
        ENGINE_ASSERT(
            window != nullptr , 
            "Failed to create SDL2 window :: {0}" ,
            SDL_GetError()
        );

        SDL_SetWindowMinimumSize(window , config.min_size.x , config.min_size.y);    

        gl_context = SDL_GL_CreateContext(window);
        ENGINE_ASSERT(gl_context != nullptr , "Failed to create OpenGL context");
        
        SDL_GL_SetSwapInterval(1);

        opened = true;
    }

    void Window::InitializeFramebuffer() {
        std::vector<float> scrn_buffer_verts = {
             1.f ,  1.f , 1.f , 1.f ,
             1.f , -1.f , 1.f , 0.f
            -1.f , -1.f , 0.f , 0.f ,
            -1.f ,  1.f , 0.f , 1.f ,
        }; 

        std::vector<uint32_t> scrn_buffer_indices = {
            0 , 3 , 1 ,
            1 , 3 , 2
        };

        framebuffer = ynew(
            Framebuffer , 
            scrn_buffer_verts , scrn_buffer_indices ,
            std::vector<uint32_t>{ 2 , 2 } ,
            size
        );
    }
    
    void Window::RegisterCallbacks() {
        /// Framebuffer resize callback
        EventManager::Instance()->RegisterWindowResizedCallback(
            [this](WindowResized* event) -> bool {
                HandleResize({ event->Width() , event->Height() });
                framebuffer->HandleResize({ event->Width() , event->Height() });
                glViewport(0 , 0 , event->Width() , event->Height());
                return true;
            } ,
            "default-window-resize"
        );
    }

    void Window::AttachShaderToFramebuffer(Shader* shader) {
        framebuffer->AttachShader(shader);
    }

    void Window::Clear() {
        // if (rendering_to_screen) {
        //     framebuffer->BindFrame();
        //     glClearColor(clear_color.r , clear_color.g , clear_color.b , clear_color.a);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //     glViewport(0 , 0 , size.x , size.y);
        // } else {
            glClearColor(clear_color.r , clear_color.g , clear_color.b , clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glViewport(0 , 0 , size.x , size.y);
        // }
    }

    void Window::Draw() {
        // if (rendering_to_screen) { 
        //     framebuffer->UnbindFrame();
        //     framebuffer->Draw();
        // }
    }
    
    void Window::SwapBuffers() {
        SDL_GL_SwapWindow(window);
    }

    void Window::Close() {
        framebuffer->Destroy();
        ydelete(framebuffer);

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        opened = false;
    }
    
}
