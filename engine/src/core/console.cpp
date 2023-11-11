#include "core/console.hpp"

#include <imgui/imgui.h>

#include "log.hpp"
#include "rendering/renderer.hpp"

namespace YE {

    std::vector<ConsoleMessage> EngineConsole::command_history;
    std::vector<ConsoleMessage> EngineConsole::message_history;
    std::vector<ConsoleMessage> EngineConsole::messages;

    void EngineConsole::ExecuteCommand(const ConsoleMessage& cmd) {
        ENTER_FUNCTION_TRACE_MSG(cmd.message);

        std::string c = cmd.message;
        if (c == "clear") { 
            message_history.insert(message_history.end() , messages.begin() , messages.end());
            messages.clear();
        }
        if (c == "dump") { Logger::Instance()->DumpBacktrace(); }
        if (c == "dump-msg") {
            for (const auto& m : message_history) {
                SinkMessage(m);
            }
        }

        EXIT_FUNCTION_TRACE();
    }
    
    void EngineConsole::Initialize() {}
    
    void EngineConsole::Update() {}
    
    void EngineConsole::DrawGui(const std::vector<ConsolePanel>& panels) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        glm::ivec2 win_size = Renderer::Instance()->ActiveWindow()->GetSize();

        ImVec4 console_bg_color = ImVec4(0x0E / 255 , 0x0E / 255 , 0x0E / 255 , 1.f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg , console_bg_color);
        ImGui::PushStyleColor(ImGuiCol_ChildBg , console_bg_color);

        ImGui::SetNextWindowSize(ImVec2(win_size.x , (win_size.y / 2) - 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Editor Console" , nullptr , window_flags)) {
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Options")) {
                    if (ImGui::MenuItem("Clear")) { messages.clear(); }
                    if (ImGui::MenuItem("Dump History")) { Logger::Instance()->DumpBacktrace(); }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            if (ImGui::BeginChild("##ConsoleOutput" , ImVec2(0, -footer_height_to_reserve))) {
                ImVec2 current_window_size = ImGui::GetContentRegionAvail();

                ImGui::Dummy(ImVec2(
                    0.f , 
                    (current_window_size.y + ImGui::GetStyle().ItemSpacing.y * messages.size()) -
                        ImGui::GetFrameHeightWithSpacing()
                ));
                
                for (const ConsoleMessage& msg : messages) {
                    ImGui::Text(msg.message.c_str());
                }

                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            for (const ConsolePanel& panel : panels) {
                panel.second({});
            }
        }
        ImGui::End();
        
        ImGui::PopStyleColor(2);
    }
    
    void EngineConsole::Shutdown() {} 

    void EngineConsole::SinkMessage(const ConsoleMessage& msg) {
        ENTER_FUNCTION_TRACE();

        if (msg.type == ConsoleMessageType::COMMAND) {
            command_history.push_back(msg);
            ExecuteCommand(msg);
        } else {
            messages.push_back(msg);
        }
        

        EXIT_FUNCTION_TRACE();
    }

} // namespace YE 