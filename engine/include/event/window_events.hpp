#ifndef YE_WINDOW_EVENTS_HPP
#define YE_WINDOW_EVENTS_HPP

#include <sstream>

#include <glm/glm.hpp>

#include "events.hpp"
#include "core/UUID.hpp"

namespace YE {

    class WindowResized : public Event {
        glm::ivec2 size;
        glm::ivec2 old_size;
        uint32_t window_id;

        public:
            WindowResized(const glm::ivec2& size , const glm::ivec2& old_size , uint32_t window_id = 0)
                : size(size) , old_size(old_size) , window_id(window_id) {}

            inline uint32_t Width() const { return size.x; }
            inline uint32_t Height() const { return size.y; }

            inline uint32_t OldWidth() const { return old_size.x; }
            inline uint32_t OldHeight() const { return old_size.y; }
            
            inline uint32_t WindowID() const { return window_id; }

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "WindowResized :: [" << old_size.x << ", " << old_size.y << "] -> [" << size.x << ", " << size.y << "]";
                return ss.str();
            }

            EVENT_TYPE(WINDOW_RESIZE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

    class WindowMinimized : public Event {
        uint32_t window_id;

        public:
            WindowMinimized(uint32_t window_id = 0) : window_id(window_id) {}

            inline uint32_t WindowID() const { return window_id; }

            EVENT_TYPE(WINDOW_MINIMIZE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

    class WindowClosed : public Event {
        uint32_t window_id;

        public:
            WindowClosed(uint32_t window_id = 0) : window_id(window_id) {}
            
            inline uint32_t WindowID() const { return window_id; }

            EVENT_TYPE(WINDOW_CLOSE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

}

#endif // !YE_WINDOW_EVENTS_HPP