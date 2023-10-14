#ifndef YE_MOUSE_EVENTS_HPP
#define YE_MOUSE_EVENTS_HPP

#include <sstream>

#include <glm/glm.hpp>

#include "events.hpp"
#include "input/mouse.hpp"

namespace YE {

    class MouseMoved : public Event {
        glm::vec2 position;
        glm::vec2 previous_position;

        public:
            MouseMoved(const glm::vec2& position , const glm::vec2& previous_position)
                : position(position) , previous_position(previous_position) {}

            inline float X() const { return position.x; }
            inline float Y() const { return position.y; }

            inline float PreviousX() const { return previous_position.x; }
            inline float PreviousY() const { return previous_position.y; }

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "MouseMoved :: [" << position.x << ", " << position.y << "]";
                return ss.str();
            }

            EVENT_TYPE(MOUSE_MOVD)
            EVENT_CATEGORY(EventCategory::MOUSE_EVNT | EventCategory::INPUT_EVENT)
    };

    class MouseScrolled : public Event {
        glm::vec2 offset;

        public:
            MouseScrolled(const glm::vec2& offset)
                : offset(offset) {}

            inline float DX() const { return offset.x; }
            inline float DY() const { return offset.y; }

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "MouseScrolled :: [" << offset.x << ", " << offset.y << "]";
                return ss.str();
            }

            EVENT_TYPE(MOUSE_SCROLLED)
            EVENT_CATEGORY(EventCategory::MOUSE_EVNT | EventCategory::INPUT_EVENT)
    };

    class MouseButton : public Event {
        protected:
            Mouse::Button button;

        public:
            MouseButton(Mouse::Button button)
                : button(button) {}

            inline Mouse::Button Button() const { return button; }

            EVENT_CATEGORY(EventCategory::MOUSE_EVNT | EventCategory::MOUSE_BUTTON_EVENT | EventCategory::INPUT_EVENT)
    };

    class MouseButtonPressed : public MouseButton {
        public:
            MouseButtonPressed(Mouse::Button button)
                : MouseButton(button) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "MouseButtonPressed :: [" << static_cast<uint8_t>(button) << "]";
                return ss.str();
            }

            EVENT_TYPE(MOUSE_BUTTON_PRESSED)
    };

    class MouseButtonReleased : public MouseButton {
        public:
            MouseButtonReleased(Mouse::Button button)
                : MouseButton(button) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "MouseButtonReleased :: [" << static_cast<uint8_t>(button) << "]";
                return ss.str();
            }

            EVENT_TYPE(MOUSE_BUTTON_RELEASED)
    };

    class MouseButtonHeld : public MouseButton {
        public:
            MouseButtonHeld(Mouse::Button button)
                : MouseButton(button) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "MouseButtonHeld :: [" << static_cast<uint8_t>(button) << "]";
                return ss.str();
            }

            EVENT_TYPE(MOUSE_BUTTON_HELD)
    };

}

#endif // !YE_MOUSE_EVENTS_HPP