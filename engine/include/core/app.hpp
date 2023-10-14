#ifndef YE_APP_HPP
#define YE_APP_HPP

#include <queue>
#include <map>
#include <string>

#include "core/window.hpp"

namespace YE {

    class App {
        std::string project_name;

        protected:
        public:
            App();
            virtual ~App() {}

            virtual WindowConfig GetWindowConfig();

            virtual void Initialize() {}
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