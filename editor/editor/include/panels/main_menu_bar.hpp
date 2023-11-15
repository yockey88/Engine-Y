#ifndef YE_EDITOR_MAIN_MENU_BAR_HPP
#define YE_EDITOR_MAIN_MENU_BAR_HPP

#include "editor_panel.hpp"

namespace editor {

    class MainMenuBar : public EditorPanel {
        public:
            MainMenuBar() {}
            ~MainMenuBar() {}

            void DrawGui(bool& open) override;
    };

} // namespace editor

#endif // !YE_EDITOR_MAIN_MENU_BAR_HPP
