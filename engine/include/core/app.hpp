#ifndef YE_APP_HPP
#define YE_APP_HPP

#include <queue>
#include <map>
#include <string>

#include "core/window.hpp"

namespace YE {

    struct EngineConfig {
        std::string project_name;
        std::string project_file_src;

        bool use_project_file = false;
    };

    class App {
        std::string project_name;

        protected:
        public:
            App();
            virtual ~App() {}

            virtual EngineConfig GetEngineConfig() { return EngineConfig(); }
            virtual WindowConfig GetWindowConfig();

            virtual bool Initialize() { return false; }
            virtual void Update(float dt) {}
            virtual void Draw() {}
            virtual void DrawGui() {}
            virtual void Shutdown() {}

            /// provides a place for users to store code they want to keep
            ///     but are not currently using
            /// \note Code stored here is still compiled and therefore effects
            ///         compile times and binary size only use this for testing
            ///         do not leave code here in production
            virtual void CodeHolder() {}

            inline const std::string& ProjectName() const { return project_name; }
    };

} // namespace YE


#endif // !YE_APP_HPP