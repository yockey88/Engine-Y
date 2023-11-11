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

    class Window {

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

        Window(Window&&) = delete;
        Window(const Window&) = delete;
        Window& operator=(Window&&) = delete;
        Window& operator=(const Window&) = delete;

        public:
            Window() {}
            ~Window();

            inline SDL_Window* GetSDLWindow() const { return window; }
            inline SDL_GLContext GetGLContext() const { return gl_context; }
            inline glm::ivec2 GetPosition() const { return position; }
            inline glm::ivec2 GetSize() const { return size; }

            void HandleResize(glm::ivec2 size);

            void Open(const WindowConfig& config);
            void RegisterCallbacks();

            void Clear();
            void SwapBuffers();
            
            void Close();

            inline const glm::vec4& ClearColor() const { return clear_color; }
            inline void SetClearColor(const glm::vec4& color) { clear_color = color; }

            inline bool Fullscreen() const { return fullscreen; }
    };

}

#endif // !YE_WINDOW_HPP