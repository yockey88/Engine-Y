#ifndef YE_GUI_HPP
#define YE_GUI_HPP

#include <unordered_map>

#include <SDL.h>

#include "core/window.hpp"
#include "core/UUID.hpp"

namespace YE {

    class Gui {
        std::unordered_map<UUID32 , Window*> windows;

        void RenderMainMenuBar(Window* window);
        void RenderMainWindow(Window* window);

        public:
            Gui() {}
            ~Gui() {}

            void Initialize(Window* main_window);
            void PushWindow(Window* window , UUID32 id);
            void Update();
            
            void BeginRender(SDL_Window* window);

            void Render(Window* window);

            void EndRender(SDL_Window* window , void* gl_context);
            void Shutdown();

            inline uint32_t WindowCount() const { return windows.size(); }
    };

}

#endif // !YE_GUI_HPP