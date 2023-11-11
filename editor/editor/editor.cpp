#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
#include "ui_utils.hpp"

class Editor : public YE::App {
    bool show_file_menu = false;

    YE::time::Timer timer{ std::chrono::milliseconds(1000) }; 
    std::unique_ptr<YE::EngineConsole> console = nullptr;
    
    char command_buffer[256] = "$> ";
    int count = 0;
    
    public:
        Editor() 
            : YE::App("editor") {}
        ~Editor() override {}

        YE::EngineConfig GetEngineConfig() override {
            YE::EngineConfig config;
            config.window_config.title = "Engine Y Editor";
            config.window_config.size.x = 1920;
            config.window_config.size.y = 1080;
            config.window_config.clear_color = {
                48 / 255.f  , 56 / 255.f , 
                126 / 255.f , 1.0f
            };
            config.window_config.vsync = true;
            config.window_config.rendering_to_screen = true;
            config.window_config.flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
            return config;
        }

        void PreInitialize() override {
            YE::Filesystem::OverrideResourcePath("editor/editor/resources");
            YE::Filesystem::OverrideProjectModulePath(
                YE::Filesystem::GetEngineRoot() +  
                "/bin/Debug/editor/editor_modules.dll"
            );
        }

        bool Initialize() override {
            EngineY::EventManager()->RegisterKeyPressedCallback(
                EditorKeyBindings , "editor-key-press"
            );

            EditorUtils::LoadImGuiStyle();

            console = std::make_unique<YE::EngineConsole>();
            
            timer.Start();

            return true;
        }

        void Update(float delta) override {
            if (!timer.Tick()) {
                LOG_DEBUG("Editor Update {}" , count);
                timer.Restart();
                count++;
            }
        }

        void Draw() override {}

        void DrawGui() override {
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
                        if (ImGui::MenuItem("Reload Shaders" , "Crtl+Shift+S")) { EngineY::ShaderReload(); }
                        if (ImGui::MenuItem("Reload Scripts" , "Crtl+Shift+R")) { EngineY::ScriptReload(); }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Tools")) {
                        ImGui::EndMenu();
                    }

                    ImGui::EndMainMenuBar();
                }
            // ImGuiWindowFlags main_win_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | 
            //                                   ImGuiWindowFlags_NoBringToFrontOnFocus;
            //if (ImGui::Begin("YE2" , nullptr , main_win_flags)) {
            // }
            // ImGui::End();

            ImGui::ShowDemoWindow();

            console->DrawGui({
                { 
                    "Command Bar" , 
                    [cmnd_buffer = command_buffer](const std::vector<void*>&) {

                        ImGui::PushStyleColor(ImGuiCol_FrameBg , ImVec4(0x0E / 255 , 0x0E / 255 , 0x0E / 255 , 1.f));
                        if (ImGui::BeginChild("##command-bar")) {
                            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll; 
                            if (ImGui::InputText("##command-bar" , cmnd_buffer , 256 , input_text_flags)) {
                                std::string input = cmnd_buffer;
                                input = input.substr(2 , input.size() - 2); // remove '$>'

                                // trim whitespace
                                size_t beginning = input.find_first_not_of(' ');
                                size_t end = input.find_last_not_of(' ');
                                size_t range = end - beginning + 1;
                                input = input.substr(beginning , range);

                                YE::EngineConsole::SinkMessage({ YE::ConsoleMessageType::COMMAND , input });
                                strcpy(cmnd_buffer , "$>  ");
                            }
                        }
                        ImGui::EndChild();
                        ImGui::PopStyleColor();
                    } 
                }
            });
        }

        void Shutdown() override {}
};

DECLARE_APP(Editor);