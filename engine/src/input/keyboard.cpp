#include "input/keyboard.hpp"

#include <memory>

#include <SDL.h>

#include "log.hpp"
#include "core/event_manager.hpp"
#include "event/events.hpp"
#include "event/keyboard_events.hpp"

namespace YE {

    std::map<Keyboard::Key , Keyboard::KeyState> Keyboard::keys{};

    void Keyboard::Initialize() {
        for (uint16_t i = 0 ; i < kKeyCount ; ++i)
            keys[static_cast<Key>(i)] = KeyState{};
    }

    void Keyboard::Update() {

        const uint8_t* state = SDL_GetKeyboardState(nullptr);

        EventManager* event_manager = EventManager::Instance();
        for (uint16_t k = 0; k < kKeyCount; ++k) {

            keys[static_cast<Key>(k)].previous_state = keys[static_cast<Key>(k)].current_state;

            if (!state[k]) {
                keys[static_cast<Key>(k)].current_state = State::RELEASED;
                keys[static_cast<Key>(k)].frames_held = 0;

                event_manager->DispatchEvent(ynew KeyReleased(static_cast<Key>(k)));
                continue;
            }

            if (state[k] && keys[static_cast<Key>(k)].current_state == State::RELEASED) {
                keys[static_cast<Key>(k)].current_state = State::PRESSED;
                ++keys[static_cast<Key>(k)].frames_held;

                event_manager->DispatchEvent(ynew KeyPressed(static_cast<Key>(k)));
                continue;
            }

            if (state[k] && keys[static_cast<Key>(k)].current_state == State::PRESSED && keys[static_cast<Key>(k)].frames_held <= 22) {
                keys[static_cast<Key>(k)].current_state = State::BLOCKED;
                ++keys[static_cast<Key>(k)].frames_held;

                continue;
            }

            if (state[k] && keys[static_cast<Key>(k)].current_state == State::BLOCKED && keys[static_cast<Key>(k)].frames_held <= 22) {
                ++keys[static_cast<Key>(k)].frames_held;
                continue;
            }

            if (keys[static_cast<Key>(k)].current_state == State::BLOCKED && keys[static_cast<Key>(k)].frames_held > 22) {
                keys[static_cast<Key>(k)].current_state = State::HELD;
                ++keys[static_cast<Key>(k)].frames_held;

                event_manager->DispatchEvent(ynew KeyHeld(static_cast<Key>(k) , keys[static_cast<Key>(k)]));
                continue;
            }

            if (state[k] && keys[static_cast<Key>(k)].current_state == State::HELD) {
                ++keys[static_cast<Key>(k)].frames_held;

                event_manager->DispatchEvent(ynew KeyHeld(static_cast<Key>(k) , keys[static_cast<Key>(k)]));
                continue;
            }
        
        }
    }

}