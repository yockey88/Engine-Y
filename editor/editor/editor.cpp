#include "EngineY.hpp"

#include "ui_utils.hpp"

class Editor : public YE::App {
    bool show_file_menu = false;

    public:
        Editor() 
            : YE::App("editor") {}
        ~Editor() override {}

        YE::EngineConfig GetEngineConfig() override {
            YE::EngineConfig config;
            config.project_name = "editor";
            config.use_project_file = false;
            return config;
        }

        YE::WindowConfig GetWindowConfig() override {
            YE::WindowConfig config;
            config.title = "Engine Y Editor";
            config.size.x = 1920;
            config.size.y = 1080;
            config.clear_color = {
                48 / 255.f  , 56 / 255.f , 
                126 / 255.f , 1.0f
            };
            config.vsync = true;
            config.rendering_to_screen = true;
            config.flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
            return config;
        }

        void PreInitialize() override {
            YE::Filesystem::OverrideResourcePath(
                YE::Filesystem::GetCWD() + "/" + 
                "editor/editor/resources"
            );
        }

        bool Initialize() override {
            EngineY::EventManager()->RegisterKeyPressedCallback(
                [](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE) {
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    }
                    return true;
                } , 
                "editor-key-press"
            );

            LoadImGuiStyle();
            
            return true;
        }

        void Update(float delta) override {}

        void Draw() override {}

        void DrawGui() override {
            
            ImGuiWindowFlags main_win_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | 
                                              ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (ImGui::Begin("YE2" , nullptr , main_win_flags)) {
                if (ImGui::BeginMainMenuBar()) {
                    if (ImGui::BeginMenu("File")) {
                        if (ImGui::MenuItem("New Project")) {}
                        if (ImGui::MenuItem("Open Project")) {}
                        if (ImGui::MenuItem("Save Project")) {}
                        ImGui::EndMenu();
                    }
                    // if (ImGui::MenuItem("Options")) { ImGui::OpenPopup("Options Menu"); }
                    // if (ImGui::MenuItem("Stats")) { ImGui::OpenPopup("Stats Popup"); } 

                    ImGui::EndMainMenuBar();
                }
            }
            ImGui::End();

            ImGui::ShowDemoWindow();

            // if (ImGui::BeginPopup("##Menu:File")) {
            //     if (ImGui::BeginCombo("##Menu:File" , "File Menu")) {
            //         if (ImGui::Selectable("New Project")) {}
            //         if (ImGui::Selectable("Open Project")) {}
            //         if (ImGui::Selectable("Save Project")) {}
            //         ImGui::EndCombo();
            //     }
            //     ImGui::EndPopup();
            // }

            // if (ImGui::BeginPopup("Options Menu")) {
            //     ImGui::Text("Options Menu");
            //     if (ImGui::MenuItem("Reload Shaders")) { EngineY::ShaderReload(); }
            //     if (ImGui::MenuItem("Reload Scripts")) { EngineY::ScriptReload(); }
            //     ImGui::EndPopup();
            // }

            // YE::EngineStats* stats = EngineY::Engine()->GetStats();
            // if (ImGui::BeginPopup("Stats Popup")) {
            //     ImGui::Text("Stats Popup");
            //     ImGui::Text("Engine Frame Stats");
            //     ImGui::Separator();
            //     ImGui::Text("FPS: %03f" , stats->last_frame_time);
            //     ImGui::SameLine();
            //     if (ImPlot::BeginPlot("Frame Times")) {
            //         ImPlot::PlotBars(
            //             "Frame Times (ms)" ,
            //             stats->frame_times , YE::kFrameTimeBufferSize //,
            //             // 1.0 , 0.0 , ImPlotLineFlags_Segments
            //         );
            //         ImPlot::EndPlot();
            //     }
            //     ImGui::EndPopup();
            // }
        }

        void Shutdown() override {}
};

DECLARE_APP(Editor)