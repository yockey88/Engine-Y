#include "core/window.hpp"

#include "log.hpp"

#include <glad/glad.h>

#include "core/resource_handler.hpp"
#include "rendering/shader.hpp"
#include "rendering/framebuffer.hpp"
#include "event/event_manager.hpp"

namespace YE {
    
    void Window::InitializeSDL2() {
        clear_color = config.clear_color;
        size = config.size;
        position = config.position;
        min_size = config.min_size;

        int sdl_init = SDL_Init(SDL_INIT_EVERYTHING);
        YE_CRITICAL_ASSERTION(sdl_init == 0 , "Failed to initialize SDL2");

        config.flags |= SDL_WINDOW_OPENGL;
        if (config.fullscreen) config.flags |= SDL_WINDOW_FULLSCREEN;
        
        window = SDL_CreateWindow(
            config.title.c_str() ,
            config.position.x , config.position.y ,
            config.size.x , config.size.y ,
            config.flags
        );
        YE_CRITICAL_ASSERTION(window != nullptr , "Failed to create SDL2 window");

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK , SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION , kOpenGLMajorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION , kOpenGLMinorVersion);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER , kOpenGLDoubleBuffer);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE , config.gl_stencil_size);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS , config.gl_multisample_buffers);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES , config.gl_multisample_samples);

        SDL_SetWindowMinimumSize(window , 200 , 200);
    }

    void Window::InitializeOpenGL() {
        gl_context = SDL_GL_CreateContext(window);
        YE_CRITICAL_ASSERTION(gl_context != nullptr , "Failed to create OpenGL context");

        SDL_GL_SetSwapInterval(1);

        int glad_init = gladLoadGLLoader(SDL_GL_GetProcAddress);
        YE_CRITICAL_ASSERTION(glad_init != 0 , "GLAD failed to initialize");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        glClearColor(config.clear_color.r , config.clear_color.g , config.clear_color.b , config.clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    Window::~Window() {
        if (opened) Close();
    }

    void Window::HandleResize(glm::ivec2 size) {
        this->size = size;
    }
    

    void Window::Open() {
        InitializeSDL2();
        InitializeOpenGL();
    }

    void Window::Clear() {
        glViewport(0 , 0 , static_cast<uint32_t>(size.x) , static_cast<uint32_t>(size.y));
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