#include "EngineY.hpp"

#include "ui_utils.hpp"

class Editor : public YE::App {
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

            if (ImGui::Begin("Test Window")) {
                ImGui::Text("YE2");
            }
            ImGui::End();
        }

        void Shutdown() override {}
};

DECLARE_APP(Editor)