#include "EngineY.hpp"

#include "editor_glue.hpp"
#include "test_native_script.hpp"

class Sandbox : public YE::App {
    YE::TextEditor text_editor;
    bool editor_open = false;

    public:
        Sandbox() {}
        virtual ~Sandbox() override {}

        virtual YE::EngineConfig GetEngineConfig() override {
            YE::EngineConfig config;
            config.window_config.title = "Engine Y";
            config.window_config.size.x = 1920;
            config.window_config.size.y = 1080;
            config.window_config.clear_color = { 0.1f , 0.1f , 0.1f , 1.f };
            config.window_config.fullscreen = false;
            config.window_config.vsync = false;
            config.window_config.rendering_to_screen = true;
            config.window_config.flags |= SDL_WINDOW_RESIZABLE;
            return config;
        }

        virtual void Initialize() override {
            EngineY::RegisterKeyPressCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE && !editor_open)
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    if (event->Key() == YE::Keyboard::Key::YE_F1) {
                        if (editor_open) {
                            text_editor.Shutdown();
                            editor_open = false;
                        } else {
                            text_editor.Initialize(
                                YE::Filesystem::GetCWD() ,
                                Zep::NVec2f(1.f , 1.f)
                            );
                            text_editor.LoadFile(YE::Filesystem::GetCWD() + "\\sandbox\\sandbox.yproj");
                            editor_open = true;
                        }
                    }
                    return true;
                } ,
                "editor-keys"
            );
        }
        
        virtual void Update(float dt) override {
            text_editor.Update();
        }

        virtual void Draw() override {
        }
        
        virtual void DrawGui() override {
            text_editor.Draw();
        }

        virtual void Shutdown() override {
            if (editor_open)
                text_editor.Shutdown();
        }
};

YE::App* CreateApp() {
    return ynew Sandbox;
}