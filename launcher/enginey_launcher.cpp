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
        }

        virtual void DrawGui() {
            ImGui::Begin("EngineY Launcher");
            ImGui::Text("Hello World!");
            ImGui::End();
        }
};

YE::App* CreateApp() {
    return ynew Launcher();
}