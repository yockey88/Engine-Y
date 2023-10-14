#ifndef YE_KEYBOARD_EVENTS_HPP
#define YE_KEYBOARD_EVENTS_HPP

#include "events.hpp"
#include "input/keyboard.hpp"

namespace YE {

    class KeyEvent : public Event {
        protected:
            Keyboard::Key key_code;

            KeyEvent(Keyboard::Key key_code)
                : key_code(key_code) {}

        public:
            inline Keyboard::Key Key() const { return key_code; }

            EVENT_CATEGORY(EventCategory::INPUT_EVENT | EventCategory::KEYBOARD_EVENT)
    };

    class KeyPressed : public KeyEvent {
        public:
            KeyPressed(Keyboard::Key key_code)
                : KeyEvent(key_code) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "Key Pressed :: " << static_cast<uint16_t>(key_code);
                return ss.str();
            }

            EVENT_TYPE(KEY_PRESSED)
    };

    class KeyReleased : public KeyEvent {
        public:
            KeyReleased(Keyboard::Key key_code)
                : KeyEvent(key_code) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "Key Released :: " << static_cast<uint16_t>(key_code);
                return ss.str();
            }

            EVENT_TYPE(KEY_RELEASED)
    };

    class KeyHeld : public KeyEvent {
        private:
            Keyboard::KeyState state;

        public:
            KeyHeld(Keyboard::Key key_code , Keyboard::KeyState state)
                : KeyEvent(key_code) , state(state) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "Key Held [ " << state.frames_held << " frames] :: " << static_cast<uint16_t>(key_code);
                return ss.str();
            }

            EVENT_TYPE(KEY_HELD)
    };

}

#endif // !YE_KEYBOARD_EVENTS_HPP