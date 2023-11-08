#include "core/window.hpp"

#include "log.hpp"

#include "core/resource_handler.hpp"
#include "rendering/shader.hpp"
#include "rendering/framebuffer.hpp"
#include "event/event_manager.hpp"

namespace YE {

    Window::~Window() {
        if (opened) Close();
    }

    void Window::HandleResize(glm::ivec2 size) {
        this->size = size;
    }

    void Window::Open(const WindowConfig& config) {
        clear_color = config.clear_color;
        size = config.size;
        position = config.position;
        min_size = config.min_size;

        window = SDL_CreateWindow(
            config.title.c_str() ,
            config.position.x , config.position.y ,
            config.size.x , config.size.y ,
            config.flags
        );
        YE_CRITICAL_ASSERTION(
            window != nullptr , 
            "Failed to create SDL2 window :: {0}" ,
            SDL_GetError()
        );

        SDL_SetWindowMinimumSize(window , config.min_size.x , config.min_size.y);    

        gl_context = SDL_GL_CreateContext(window);
        YE_CRITICAL_ASSERTION(gl_context != nullptr , "Failed to create OpenGL context");
        
        SDL_GL_SetSwapInterval(1);

        opened = true;
    }

    void Window::RegisterCallbacks() {
        /// Framebuffer resize callback
        EventManager::Instance()->RegisterWindowResizedCallback(
            [window = this](WindowResized* event) -> bool {
                window->HandleResize({ event->Width() , event->Height() });
                glViewport(0 , 0 , event->Width() , event->Height());
                return true;
            } ,
            "default-window-resize"
        );
    }

    void Window::Clear() {
        glClearColor(clear_color.r , clear_color.g , clear_color.b , clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void Window::SwapBuffers() {
        SDL_GL_SwapWindow(window);
    }

    void Window::Close() {
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        opened = false;
    }
    
}