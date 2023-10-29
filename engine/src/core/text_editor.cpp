#include "core/text_editor.hpp"

#include <imgui/imgui.h>
#include <zep/imgui/display_imgui.h>

namespace YE {

    void TextEditor::Initialize(const std::string& path) {
        std::filesystem::path p = path;
        console = std::make_unique<Zep::ZepConsole>(p);
        editor = std::make_unique<Zep::ZepEditor_ImGui>(p , Zep::NVec2f(1.0f , 1.0f));

        auto& display = console->GetEditor().GetDisplay();
        auto im_font = ImGui::GetIO().Fonts[0].Fonts[0];
        auto pixelh = im_font->FontSize;

        display.SetFont(Zep::ZepTextType::UI , std::make_shared<Zep::ZepFont_ImGui>(display , im_font , (uint32_t)pixelh));
        display.SetFont(Zep::ZepTextType::Text , std::make_shared<Zep::ZepFont_ImGui>(display , im_font , (uint32_t)pixelh));
        display.SetFont(Zep::ZepTextType::Heading1 , std::make_shared<Zep::ZepFont_ImGui>(display , im_font , (uint32_t)pixelh * 1.5));
        display.SetFont(Zep::ZepTextType::Heading2 , std::make_shared<Zep::ZepFont_ImGui>(display , im_font , (uint32_t)pixelh * 1.25));
        display.SetFont(Zep::ZepTextType::Heading3 , std::make_shared<Zep::ZepFont_ImGui>(display , im_font , (uint32_t)pixelh * 1.125));
    }

    void TextEditor::Update() {
        if (console == nullptr) return;

        console->GetEditor().RefreshRequired();
    }

    void TextEditor::Draw() {
        bool show = true;
        console->Draw("Console" , &show , ImVec4(0 , 0 , 500 , 400) , true);
        console->AddLog("Hello Zep Console!");

        ImGui::SetNextWindowSize(ImVec2(500 , 400) , ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Zep" , &show , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        auto min = ImGui::GetCursorScreenPos();
        auto max = ImGui::GetContentRegionAvail();

        if (max.x <= 0) max.x = 1;
        if (max.y <= 0) max.y = 1;
        ImGui::InvisibleButton("ZepContainer" , max);

        max.x = min.x + max.x;
        max.y = min.y + max.y;

        console->zepEditor.SetDisplayRegion(Zep::NVec2f(min.x , min.y) , Zep::NVec2f(max.x , max.y));
        console->zepEditor.Display();

        bool zep_focused = ImGui::IsWindowFocused();
        if (zep_focused) {
            console->zepEditor.HandleInput();
        }

        static uint32_t focus_tick = 0;
        if (focus_tick++ < 2) {
            ImGui::SetWindowFocus();
        }

        ImGui::End();
    }

    void TextEditor::Shutdown() {
        console = nullptr;
        editor = nullptr;
    }

} // namespace YE