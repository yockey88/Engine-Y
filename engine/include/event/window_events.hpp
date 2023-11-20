#ifndef ENGINEY_WINDOW_EVENTS_HPP
#define ENGINEY_WINDOW_EVENTS_HPP

#include <sstream>

#include <glm/glm.hpp>

#include "events.hpp"
#include "core/UUID.hpp"

namespace EngineY {

    class WindowResized : public Event {
        glm::ivec2 size;
        glm::ivec2 old_size;

        public:
            WindowResized(const glm::ivec2& size , const glm::ivec2& old_size)
                : size(size) , old_size(old_size) {}

            inline uint32_t Width() const { return size.x; }
            inline uint32_t Height() const { return size.y; }

            inline uint32_t OldWidth() const { return old_size.x; }
            inline uint32_t OldHeight() const { return old_size.y; }

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "WindowResized :: [" << old_size.x << ", " << old_size.y << "] -> [" << size.x << ", " << size.y << "]";
                return ss.str();
            }

            EVENT_TYPE(WINDOW_RESIZE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

    class WindowMinimized : public Event {
        public:
            WindowMinimized() {}

            EVENT_TYPE(WINDOW_MINIMIZE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

    class WindowClosed : public Event {
        public:
            WindowClosed() {}

            EVENT_TYPE(WINDOW_CLOSE)
            EVENT_CATEGORY(EventCategory::WINDOW_EVENT | EventCategory::APPLICATION_EVENT)
    };

}

#endif // !YE_WINDOW_EVENTS_HPP