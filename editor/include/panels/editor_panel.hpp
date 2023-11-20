#ifndef ENGINEY_EDITOR_PANEL_HPP
#define ENGINEY_EDITOR_PANEL_HPP

#include <string>

#include "event/events.hpp"

namespace EngineY {

    class Scene;

} // namespace YE

namespace editor {

    class EditorPanel {
        public:
            virtual ~EditorPanel() {}

            virtual void DrawGui(bool& open) = 0;
            virtual void Event( EngineY::Event& event) {}
            virtual void ProjectChanged() {}
            virtual void SetSceneContext( EngineY::Scene* scene) {}
    };

} // namespace editor

#endif // !YE_EDITOR_PANEL_HPP
