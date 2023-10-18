#include "EngineY.hpp"

class Launcher : public YE::App {
    bool project_config_open = false;
    char project_name[256] = { 0 };

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
                        project_config_open = true;
                    }
                    ImGui::EndCombo();
                }

                if (project_config_open) {
                    std::string project_path = "projects/";
                    if (ImGui::BeginChild("Project Builder")) {
                        ImGui::InputText("Project Name" , project_name , 256);

                        if (ImGui::Button("Create")) {
                            project_path += project_name;
                            if (std::filesystem::exists(project_path)) {
                                YE_WARN("Project already exists!");
                            } else {
                                std::filesystem::create_directory(project_path);
                            }
                            project_config_open = false;
                        }
                        ImGui::EndChild();
                    }
                }

            }
            ImGui::End();
        }
};

YE::App* CreateApp() {
    return ynew Launcher();
}