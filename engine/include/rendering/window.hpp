#ifndef ENGINEY_WINDOW_HPP
#define ENGINEY_WINDOW_HPP

#include <string>

#include <SDL.h>
#undef main
#include <glm/glm.hpp>

#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "application/application_config.hpp"

namespace EngineY {

    class VertexArray;
    class Shader;
    class Framebuffer;

    class Window {

        glm::vec4 clear_color = glm::vec4(0.2f , 0.3f , 0.3f , 1.0f);
        glm::ivec2 position = glm::ivec2(0 , 0);
        glm::ivec2 min_size = glm::ivec2(200 , 200);
        glm::ivec2 size = glm::ivec2(800 , 600);

        bool opened = false;
        bool shader_attached = false;
        bool rendering_to_screen = true;
        bool fullscreen = false;

        SDL_Window* window = nullptr;
        SDL_GLContext gl_context = nullptr;

        VertexArray* quad_vao = nullptr;
        Framebuffer* framebuffer = nullptr;


        public:
            Window() {}
            ~Window() {}

            inline SDL_Window* GetSDLWindow() const { return window; }
            inline SDL_GLContext GetGLContext() const { return gl_context; }
            inline glm::ivec2 GetPosition() const { return position; }
            inline glm::ivec2 GetSize() const { return size; }

            void HandleResize(glm::ivec2 size);

            void Open(const WindowConfig& config);
            void InitializeFramebuffer();
            void RegisterCallbacks();

            void AttachShaderToFramebuffer(Shader* shader); 
            void Clear();
            void Draw();
            void SwapBuffers();
            
            void Close();

            Framebuffer* GetFramebuffer() const { return framebuffer; }
            inline const glm::vec4& ClearColor() const { return clear_color; }
            inline void SetClearColor(const glm::vec4& color) { clear_color = color; }
            inline bool Fullscreen() const { return fullscreen; }
            inline bool RenderingToScreen() const { return rendering_to_screen; }
    };

}

#endif // !YE_WINDOW_HPP
