#ifndef ENGINEY_EDITOR_SCENE_PANEL_HPP
#define ENGINEY_EDITOR_SCENE_PANEL_HPP

#include <string>

#include "editor_panel.hpp"

namespace EngineY {

    class Entity;
    class Scene;

} // namespace YE

namespace editor {

    class ScenePanel : public EditorPanel {
         EngineY::Scene* context;
       
        // bool is_window = true;
        bool currently_focused = false;
        bool hierarchy_or_props_focused = false;
        bool shift_select_running = false;
        // bool searching = false;

        void DrawEntity( EngineY::Entity* entity);

        public:
            ScenePanel() {}
            ScenePanel( EngineY::Scene* scene) : context(scene) {}
            ~ScenePanel() override {}
    
            void DrawGui(bool& open) override;
            inline void SetSceneContext( EngineY::Scene* scene) override { context = scene; }
    };

} // namespace editor

#endif // !YE_EDITOR_SCENE_PANEL_HPP
