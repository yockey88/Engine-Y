#ifndef ENGINEY_APP_HPP
#define ENGINEY_APP_HPP

#include <queue>
#include <map>
#include <string>

#include "core/defines.hpp"
#include "rendering/window.hpp"

namespace EngineY {

    class App;

    typedef App*(*CreateAppFunc)();

    class App {
        protected:
            std::string project_name;

        public:
            App(const std::string& project_name)
                : project_name(project_name) {}
            virtual ~App() {}

            virtual void PreInitialize() {}
            virtual bool Initialize() { return false; }

            virtual void Start() {}

            virtual void Update(float dt) {}
            virtual void Draw() {}

            virtual void DrawGui() {}
           
            virtual void Stop() {}

            virtual void Shutdown() {}

            inline const std::string& ProjectName() const { return project_name; }
    };

} // namespace YE


#endif // !YE_APP_HPP
