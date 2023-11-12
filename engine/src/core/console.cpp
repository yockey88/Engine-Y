#include "core/console.hpp"

#include <imgui/imgui.h>

#include "log.hpp"
#include "core/filesystem.hpp"
#include "core/resource_handler.hpp"
#include "rendering/renderer.hpp"

namespace YE {

    std::vector<ConsoleMessage> EngineConsole::command_history;
    std::vector<ConsoleMessage> EngineConsole::message_history;
    std::vector<ConsoleMessage> EngineConsole::messages;

    char EngineConsole::cmnd_buffer[kCmndBufferSize] = "$>  "; 

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
    
    void EngineConsole::Initialize() {
        ENTER_FUNCTION_TRACE();

        is_initialized = true;

        EXIT_FUNCTION_TRACE();
    }
    
    void EngineConsole::Update() {}
    
    void EngineConsole::DrawGui() {
        if (!is_initialized) {
            Initialize();
        }

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        glm::ivec2 win_size = Renderer::Instance()->ActiveWindow()->GetSize();

        ImVec4 console_bg_color = ImVec4(0x0E / 255 , 0x0E / 255 , 0x0E / 255 , 1.f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg , console_bg_color);
        ImGui::PushStyleColor(ImGuiCol_ChildBg , console_bg_color);

        ImFont* blex = ResourceHandler::Instance()->GetCoreFont("BlexMono");
        if (blex != nullptr) {
            ImGui::PushFont(blex);
        } else {
            LOG_WARN("Failed to load BlexMono font");
        }

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

            ImGui::PushStyleColor(ImGuiCol_FrameBg , ImVec4(0x0E / 255 , 0x0E / 255 , 0x0E / 255 , 1.f));
            // ImGui::PushFont()
            if (ImGui::BeginChild("##command-bar")) {
                ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll; 
                if (ImGui::InputText("##command-bar" , cmnd_buffer , kCmndBufferSize , input_text_flags)) {
                    std::string input = cmnd_buffer;
                    input = input.substr(2 , input.size() - 2); // remove '$>'

                    // trim whitespace
                    size_t beginning = input.find_first_not_of(' ');
                    size_t end = input.find_last_not_of(' ');
                    size_t range = end - beginning + 1;
                    input = input.substr(beginning , range);

                    YE::EngineConsole::SinkMessage({ YE::ConsoleMessageType::COMMAND , input });
                    std::strcpy(cmnd_buffer , "$>  ");    
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::End();

        if (blex != nullptr) ImGui::PopFont();
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