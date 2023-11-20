#ifndef ENGINEY_SCENE_EVENTS_HPP
#define ENGINEY_SCENE_EVENTS_HPP

#include "events.hpp"
#include "scene/scene.hpp"

namespace EngineY {

    class SceneEvent : public Event {
        protected:
            Scene* context;

        public:
            SceneEvent(Scene* context)
                : context(context) {}
            
            inline Scene* Context() const { return context; }

            EVENT_CATEGORY(EventCategory::SCENE_EVENT)
    };

    class SceneLoad : public SceneEvent {
        public:
            SceneLoad(Scene* context)
                : SceneEvent(context) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "SceneLoad :: [" << context->SceneName() << "]";
                return ss.str();
            }

            EVENT_TYPE(SCENE_LOAD)
    };

    class SceneStart : public SceneEvent {
        public:
            SceneStart(Scene* context)
                : SceneEvent(context) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "SceneInitialize :: [" << context->SceneName() << "]";
                return ss.str();
            }

            EVENT_TYPE(SCENE_START)
    };

    class SceneStop : public SceneEvent {
        public:
            SceneStop(Scene* context)
                : SceneEvent(context) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "SceneStop :: [" << context->SceneName() << "]";
                return ss.str();
            }

            EVENT_TYPE(SCENE_STOP)
    };

    class SceneUnload : public SceneEvent {
        public:
            SceneUnload(Scene* context)
                : SceneEvent(context) {}

            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << "SceneUnload :: [" << context->SceneName() << "]";
                return ss.str();
            }

            EVENT_TYPE(SCENE_UNLOAD)
    };

}

#endif // !YE_SCENE_EVENTS_HPP