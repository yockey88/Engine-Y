#include "ui_utils.hpp"

void LoadImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Border] = EditorStyles::clear_color;
    style.Colors[ImGuiCol_TabActive] = EditorStyles::tab_hovered;
    style.Colors[ImGuiCol_TabHovered] = EditorStyles::tab;
    style.GrabRounding = 2.3f;
}