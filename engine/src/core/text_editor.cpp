#include "core/text_editor.hpp"

#include <imgui/imgui.h>

#include "log.hpp"

namespace YE {

    void TextEditor::Initialize(const std::string& path , Zep::NVec2f pixel_scale) {
        zep = std::make_unique<ZepWrapper>(
            std::filesystem::path(path) , 
            Zep::NVec2f(pixel_scale.x , pixel_scale.y) ,
            [&](std::shared_ptr<Zep::ZepMessage> message) -> void {
                if (message->str != "") 
                    YE_INFO("Editor Message :: {}" , message->str);
            }
        );

        auto& display = zep->GetEditor().GetDisplay();
        auto font = ImGui::GetIO().Fonts[0].Fonts[0];
        auto pixel_height = font->FontSize;

        display.SetFont(Zep::ZepTextType::UI , std::make_shared<Zep::ZepFont_ImGui>(display , font , (uint32_t)pixel_height));
        display.SetFont(Zep::ZepTextType::Text , std::make_shared<Zep::ZepFont_ImGui>(display , font , (uint32_t)pixel_height));
        display.SetFont(Zep::ZepTextType::Heading1 , std::make_shared<Zep::ZepFont_ImGui>(display , font , (uint32_t)pixel_height * 1.5));
        display.SetFont(Zep::ZepTextType::Heading2 , std::make_shared<Zep::ZepFont_ImGui>(display , font , (uint32_t)pixel_height * 1.25));
        display.SetFont(Zep::ZepTextType::Heading3 , std::make_shared<Zep::ZepFont_ImGui>(display , font , (uint32_t)pixel_height * 1.125));
    }

    void TextEditor::LoadFile(const std::filesystem::path& path) {
        if (zep == nullptr)
            return;

        zep->GetEditor().InitWithFileOrDir(path.string());
    }

    void TextEditor::Update() {
        if (zep != nullptr)
            zep->GetEditor().RefreshRequired();
    }

    void TextEditor::Draw() {
        if (zep == nullptr)
            return;

        bool show = true;

        ImGui::SetNextWindowSize(ImVec2(640 , 480) , ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Editor" , &show , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        auto min = ImGui::GetCursorScreenPos();
        auto max = ImGui::GetContentRegionAvail();

        if (max.x <= 0) max.x = 1;
        if (max.y <= 0) max.y = 1;

        ImGui::InvisibleButton("EditorContainer" , max);

        max.x = min.x + max.x;
        max.y = min.y + max.y;

        zep->editor.SetDisplayRegion(Zep::NVec2f(min.x , min.y) , Zep::NVec2f(max.x , max.y));
        zep->editor.Display();

        if (ImGui::IsWindowFocused()) {
            zep->HandleInput();
        
        }

        static uint32_t focus = 0;
        if (focus++ < 2) {
            ImGui::SetWindowFocus();
        }
        
        ImGui::End();
    }

    void TextEditor::Shutdown() {
        zep = nullptr;
    }

} // namespace YE