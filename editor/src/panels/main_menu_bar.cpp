#include "panels/main_menu_bar.hpp"

#include <imgui/imgui.h>

#include "core/resource_handler.hpp"
#include "scripting/script_engine.hpp"

namespace editor {

    void MainMenuBar::DrawGui(bool& open) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Project" , "Ctrl+N")) {}
                if (ImGui::MenuItem("Open Project")) {}
                if (ImGui::MenuItem("Save Project")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Reload Shaders" , "Crtl+Shift+S")) { 
                    ENGINE_DEBUG("Reloading Shaders");
                    EngineY::ResourceHandler::Instance()->ReloadShaders();
                }
                if (ImGui::MenuItem("Reload Scripts" , "Crtl+Shift+R")) { 
                    EngineY::ScriptEngine::Instance()->ReloadProjectModules();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools")) {
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    } 

} // namespace editor
