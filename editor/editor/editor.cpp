#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
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
                EditorKeyBindings , "editor-key-press"
            );

            LoadImGuiStyle();
            
            return true;
        }

        void Update(float delta) override {}

        void Draw() override {}

        void DrawGui() override {
            ImGuiWindowFlags main_win_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | 
                                              ImGuiWindowFlags_NoBringToFrontOnFocus;
            //if (ImGui::Begin("YE2" , nullptr , main_win_flags)) {
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
            // }
            // ImGui::End();
        }

        void Shutdown() override {}
};

DECLARE_APP(Editor)