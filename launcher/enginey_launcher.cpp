#include "EngineY.hpp"

class Launcher : public YE::App {
    public:
        Launcher() : YE::App() {}
        virtual ~Launcher() {}

        virtual void RenderGui() {
            ImGui::Begin("EngineY Launcher");
            ImGui::Text("Hello World!");
            ImGui::End();
        }
};

YE::App* CreateApp() {
    return new Launcher();
}