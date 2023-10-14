#ifndef YE_EDITOR_HPP
#define YE_EDITOR_HPP

#include "scene/scene.hpp"

namespace YE {

    class App;
    class Engine;

    class Editor {
        Engine* engine = nullptr;
        App* app = nullptr;

        public:
            Editor(Engine* engine = nullptr)
                : engine(engine) {}
            ~Editor() {}

            void Initialize(App* app);
            void Update();
            void Render();
            void Shutdown();
    };

}

#endif // !YE_EDITOR_HPP