#ifndef YE_EVENTS_HPP
#define YE_EVENTS_HPP

#include <string>
#include <functional>

#include "core/defines.hpp"

namespace YE {

    enum class EventType {
        EMPTY = 0 ,
        WINDOW_CLOSE , WINDOW_MINIMIZE , WINDOW_RESIZE , WINDOW_FOCUS , WINDOW_LOST_FOCUS , WINDOW_MOVED ,
        APP_TICK , APP_UPDATE , APP_RENDER ,
        KEY_PRESSED , KEY_RELEASED , KEY_TYPED , KEY_HELD ,
        MOUSE_BUTTON_PRESSED , MOUSE_BUTTON_RELEASED , MOUSE_BUTTON_HELD , MOUSE_MOVD , MOUSE_SCROLLED ,
        SCENE_LOAD , SCENE_START , SCENE_STOP , SCENE_UNLOAD ,
        EDITOR_SCENE_PLAY , EDITOR_SCENE_PAUSE , EDITOR_SCENE_STOP ,
        SHUTDOWN
    };

    enum EventCategory {
        NONE = 0 ,
        APPLICATION_EVENT  = ybit(0) ,
        WINDOW_EVENT       = ybit(1) ,
        INPUT_EVENT        = ybit(2) ,
        KEYBOARD_EVENT     = ybit(3) ,
        MOUSE_EVNT         = ybit(4) ,
        MOUSE_BUTTON_EVENT = ybit(5) ,
        SCENE_EVENT        = ybit(6) ,
        EDITOR_EVENT       = ybit(7) ,
        SHUTDOWN_EVENT     = ybit(8)
    };

#define EVENT_TYPE(type) virtual EventType Type() const { return EventType::##type; } \
                         virtual std::string Name() const override { return std::string{ #type }; }

#define EVENT_CATEGORY(category) virtual uint32_t CategoryFlags() const override { return category; }

    class Event {
        public:
            bool handled = false;

            Event() {}
            virtual ~Event() {}
            virtual EventType Type() const = 0;
            virtual std::string Name() const = 0;
            virtual uint32_t CategoryFlags() const = 0;
            virtual std::string ToString() const { return Name(); }

            inline bool InCategory(EventCategory category) { return CategoryFlags() & category; }
    };

    inline std::ostream& operator<<(std::ostream& os , const Event& e) {
        return os << e.Name();
    }

    /**
     * Might be able to eliminate this class
    */
    class EventHandler {
        template<typename T>
        using EventFunc = std::function<bool(T&)>; 

        Event* event = nullptr;

        public:
            EventHandler(Event* event)
                : event(event) {}

            template<typename T>
            bool Handle(EventFunc<T> func) {
                if (event == nullptr) return false;
                if (event->Type() == T::Type() && !event->handled) {
                    event->handled = func(*(T*)event);
                    return true;
                }
                return false;
            }
    };

    class ShutdownEvent : public Event {
        public:
            ShutdownEvent() {}

            EVENT_TYPE(SHUTDOWN)
            EVENT_CATEGORY(EventCategory::SHUTDOWN_EVENT)
    };

}

#endif