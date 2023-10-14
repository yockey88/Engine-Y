#include "input/mouse.hpp"

#include <memory>

#include "core/window.hpp"
#include "event/events.hpp"
#include "event/mouse_events.hpp"
#include "event/event_manager.hpp"
#include "rendering/renderer.hpp"

namespace YE {

    Mouse::MouseState Mouse::state{};
    std::map<Mouse::Button , Mouse::ButtonState> Mouse::buttons{};

    void Mouse::Initialize() {
        Window* window = Renderer::Instance()->ActiveWindow();
        if (window != nullptr) {
            SDL_Window* sdl_window = window->GetSDLWindow();
            glm::ivec2 window_size = window->GetSize();
            SDL_WarpMouseInWindow(sdl_window , window_size.x / 2 , window_size.y / 2);
        }

        for (uint8_t i = 0; i < kButtonCount; ++i)
            buttons[static_cast<Button>(i)] = ButtonState{};
    }

    void Mouse::Update() {
        state.previous_position = state.position;

        uint32_t curr_state = SDL_GetMouseState(&state.position.x , &state.position.y);

        SDL_Window* window = SDL_GetMouseFocus();
        state.in_window = window != nullptr;

        EventManager* event_manager = EventManager::Instance();
        for (uint32_t b = 0; b < kButtonCount; ++b) {
            buttons[static_cast<Button>(b)].previous_state = buttons[static_cast<Button>(b)].current_state;

            if (!(curr_state & SDL_BUTTON(b))) {
                buttons[static_cast<Button>(b)].current_state = State::RELEASED;
                buttons[static_cast<Button>(b)].frames_held = 0;

                event_manager->DispatchEvent(new MouseButtonReleased(static_cast<Button>(b)));
                continue;
            }
            
            if (curr_state & SDL_BUTTON(b) && buttons[static_cast<Button>(b)].current_state == State::RELEASED) {
                buttons[static_cast<Button>(b)].current_state = State::PRESSED;
                ++buttons[static_cast<Button>(b)].frames_held;

                event_manager->DispatchEvent(new MouseButtonPressed(static_cast<Button>(b)));
                continue;
            }
            
            if (curr_state & SDL_BUTTON(b) && buttons[static_cast<Button>(b)].current_state == State::PRESSED &&
                buttons[static_cast<Button>(b)].frames_held <= 22) {
                buttons[static_cast<Button>(b)].current_state = State::BLOCKED;
                ++buttons[static_cast<Button>(b)].frames_held;
                continue;
            }

            if (curr_state & SDL_BUTTON(b) && buttons[static_cast<Button>(b)].current_state == State::BLOCKED &&
                buttons[static_cast<Button>(b)].frames_held <= 22) {
                ++buttons[static_cast<Button>(b)].frames_held;
                continue;
            }

            if (curr_state & SDL_BUTTON(b) && buttons[static_cast<Button>(b)].current_state == State::BLOCKED && 
                buttons[static_cast<Button>(b)].frames_held > 22) {
                buttons[static_cast<Button>(b)].current_state = State::HELD;
                ++buttons[static_cast<Button>(b)].frames_held;
                continue;
            }

            if (curr_state & SDL_BUTTON(b) && buttons[static_cast<Button>(b)].current_state == State::HELD) {
                ++buttons[static_cast<Button>(b)].frames_held;

                event_manager->DispatchEvent(new MouseButtonHeld(static_cast<Button>(b)));
                continue;
            }
        }
    }
    
    void Mouse::SnapToCenter() {
        SDL_Window* window = SDL_GetMouseFocus();
        if (window == nullptr)
            return;

        Window* ye_window = Renderer::Instance()->ActiveWindow();
        if (ye_window == nullptr)
            return;

        glm::ivec2 window_size = ye_window->GetSize();

        SDL_WarpMouseInWindow(window , window_size.x / 2 , window_size.y / 2);
    }

    void Mouse::FreeCursor() {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void Mouse::HideCursor() {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

}