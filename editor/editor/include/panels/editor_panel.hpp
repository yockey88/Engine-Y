#ifndef YE_EDITOR_PANEL_HPP
#define YE_EDITOR_PANEL_HPP

#include <string>

#include "event/events.hpp"

namespace YE {

    class Scene;

} // namespace YE

namespace editor {

    class EditorPanel {
        public:
            virtual ~EditorPanel() {}

            virtual void DrawGui(bool& open) = 0;
            virtual void Event(YE::Event& event) {}
            virtual void ProjectChanged() {}
            virtual void SetSceneContext(YE::Scene* scene) {}
    };

} // namespace editor

#endif // !YE_EDITOR_PANEL_HPP
