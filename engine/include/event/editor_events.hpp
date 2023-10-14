#ifndef YE_EDITOR_EVENTS_HPP
#define YE_EDITOR_EVENTS_HPP

#include "events.hpp"

namespace YE {

    class EditorEvent : public Event {
        public:
            EditorEvent() {}

            EVENT_CATEGORY(EventCategory::EDITOR_EVENT)
    };

    class EditorPlay : public EditorEvent {
        public:
            EditorPlay() {}

            EVENT_TYPE(EDITOR_SCENE_PLAY)
    };

    class EditorPause : public EditorEvent {
        public:
            EditorPause() {}

            EVENT_TYPE(EDITOR_SCENE_PAUSE)
    };

    class EditorStop : public EditorEvent {
        public:
            EditorStop() {}

            EVENT_TYPE(EDITOR_SCENE_STOP)
    };

}

#endif // !YE_EDITOR_EVENTS_HPP