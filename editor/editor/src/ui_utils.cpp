#include "ui_utils.hpp"

namespace EditorUtils {

    void LoadImGuiStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TitleBg]            = EditorStyles::title_bar;
        style.Colors[ImGuiCol_TitleBgActive]      = EditorStyles::title_bar_active;
        style.Colors[ImGuiCol_TitleBgCollapsed]   = EditorStyles::title_bar_collapsed;
        style.Colors[ImGuiCol_MenuBarBg]          = EditorStyles::menu_bar;
        style.Colors[ImGuiCol_Tab]                = EditorStyles::tab;
        style.Colors[ImGuiCol_TabHovered]         = EditorStyles::tab_hovered;
        style.Colors[ImGuiCol_TabActive]          = EditorStyles::tab;
        style.Colors[ImGuiCol_TabUnfocused]       = EditorStyles::tab_unfocused;
        style.Colors[ImGuiCol_TabUnfocusedActive] = EditorStyles::tab_unfocused_active;
        style.GrabRounding = 2.3f;
    }

} // namespace EditorUtils