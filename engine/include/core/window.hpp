#ifndef YE_WINDOW_HPP
#define YE_WINDOW_HPP

#include <string>

#include <SDL.h>
#undef main
#include <glm/glm.hpp>

#include "defines.hpp"
#include "UUID.hpp"

namespace YE {

    class Shader;
    class Framebuffer;

    struct WindowConfig {
        glm::ivec2 position = glm::ivec2(SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED);
        glm::ivec2 min_size = glm::ivec2(200 , 200);
        glm::ivec2 size = glm::ivec2(800 , 600);
        glm::vec4 clear_color = glm::vec4(0.2f , 0.3f , 0.3f , 1.0f);
        uint32_t flags = 0;
        uint32_t gl_stencil_size = 8;
        uint32_t gl_multisample_buffers = 1;
        uint32_t gl_multisample_samples = 24;
        std::string title = "Engine Y";
        
        bool fullscreen = false;
        bool vsync = false;
        bool rendering_to_screen = true;
    };

    class Window {

        const uint32_t kOpenGLMajorVersion = 4;
        const uint32_t kOpenGLMinorVersion = 6;
        const uint32_t kOpenGLDoubleBuffer = 1;
        const uint32_t kOpenGLSwapInterval = 1;

        WindowConfig config;

        glm::vec4 clear_color = glm::vec4(0.2f , 0.3f , 0.3f , 1.0f);
        glm::ivec2 position = glm::ivec2(0 , 0);
        glm::ivec2 min_size = glm::ivec2(200 , 200);
        glm::ivec2 size = glm::ivec2(800 , 600);

        bool opened = false;
        bool shader_attached = false;
        bool framebuffer_attached = false;
        bool rendering_to_screen = true;
        bool fullscreen = false;

        SDL_Window* window = nullptr;
        SDL_GLContext gl_context = nullptr;

        void InitializeSDL2();
        void InitializeOpenGL();

        Window(Window&&) = delete;
        Window(const Window&) = delete;
        Window& operator=(Window&&) = delete;
        Window& operator=(const Window&) = delete;

        public:
            Window(const WindowConfig& config) 
                : config(config) {}
            ~Window();

            inline SDL_Window* GetSDLWindow() const { return window; }
            inline SDL_GLContext GetGLContext() const { return gl_context; }
            inline glm::ivec2 GetPosition() const { return position; }
            inline glm::ivec2 GetSize() const { return size; }

            void HandleResize(glm::ivec2 size);

            void Open();

            void Clear();
            void SwapBuffers();
            
            void Close();

            inline const glm::vec4& ClearColor() const { return clear_color; }
            inline void SetClearColor(const glm::vec4& color) { clear_color = color; }

            inline bool Fullscreen() const { return fullscreen; }
    };

}

#endif // !YE_WINDOW_HPP