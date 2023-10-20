#include "EngineY.hpp"

class Sandbox : public YE::App {
    public:
        Sandbox() {}
        virtual ~Sandbox() override {}

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
            EngineY::EventManager()->RegisterKeyPressedCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE) 
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent());
                    if (event->Key() == YE::Keyboard::Key::YE_W) {
                        YE::WindowConfig config;
                        config.title = "Test Window";
                        config.size.x = 800;
                        config.size.y = 600;

                        config.clear_color = { 0.6f , 0.6f , 0.8f , 1.f };
                        config.fullscreen = false;
                        config.vsync = false;
                        config.rendering_to_screen = true;
                        config.flags |= SDL_WINDOW_RESIZABLE;
                        EngineY::Renderer()->OpenWindow(config);
                    }
                    return true;
                } ,
                "key-callback"
            );
        }
        
        virtual void Update(float dt) override {}

        virtual void Draw() override {}
        
        virtual void DrawGui() override { }

        virtual void Shutdown() override {}
};

YE::App* CreateApp() {
    return ynew Sandbox;
}