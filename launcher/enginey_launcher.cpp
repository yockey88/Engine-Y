#include "EngineY.hpp"

class Launcher : public YE::App {
    public:
        Launcher() : YE::App() {}
        virtual ~Launcher() {}

        virtual std::string Initialize() {
            std::cout << ProjectName() << std::endl;
        }

        virtual void RenderGui() {
            ImGui::Begin("EngineY Launcher");
            ImGui::Text("Hello World!");
            ImGui::End();
        }
};

YE::App* CreateApp() {
    return new Launcher();
}