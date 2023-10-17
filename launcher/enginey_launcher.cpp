#include "EngineY.hpp"

class Launcher : public YE::App {
    public:
        Launcher() : YE::App() {}
        virtual ~Launcher() {}
        
        virtual YE::WindowConfig GetWindowConfig() override {
            YE::WindowConfig config;
            config.title = "Engine Y";
            config.size.x = 1920;
            config.size.y = 1080;
            config.clear_color = { 0.1f , 0.1f , 0.1f , 1.f };
            config.fullscreen = false;
            config.vsync = false;
            config.rendering_to_screen = true;
            config.flags |= SDL_WINDOW_RESIZABLE;
            return config;
        }

        virtual void Initialize() override {
            EngineY::RegisterKeyPressCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE)
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    return true;
                } ,
                "editor-keys"
            );

            if (!std::filesystem::exists("projects")) {
                std::filesystem::create_directory("projects");
            }
        }

        virtual void DrawGui() {
            if (ImGui::Begin("Engine Y Project Launcher")) {
                for (auto& entry : std::filesystem::directory_iterator("projects")) {
                    if (!entry.is_directory()) continue;
                    if (ImGui::Button(entry.path().filename().string().c_str())) {
                        std::string project_name = entry.path().filename().string();
                        // EngineY::LoadProject(project_name);
                        ImGui::Text("%s" , project_name.c_str());
                    }
                }

                if (ImGui::BeginCombo("Create Project" , "Select")) {
                    if (ImGui::Selectable("Empty Project")) {
                        ImGui::OpenPopup("Create Project");
                    }
                    ImGui::EndCombo();
                }

            }
            ImGui::End();
        }
};

YE::App* CreateApp() {
    return ynew Launcher();
}