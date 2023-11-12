#ifndef YE_GUI_HPP
#define YE_GUI_HPP

#include <map>

#include <SDL.h>
#include <imgui/imgui.h>

#include "core/window.hpp"

namespace YE {
    
    class Gui {
        public:
            Gui() {}
            ~Gui() {}

            void Initialize(Window* window);
            void Update();
            
            void BeginRender(SDL_Window* window);

            void Render(Window* window);

            void EndRender(SDL_Window* window , void* gl_context);
            void Shutdown();
    };

}

#endif // !YE_GUI_HPP