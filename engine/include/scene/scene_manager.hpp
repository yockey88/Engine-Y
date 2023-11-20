#ifndef ENGINEY_SCENE_MANAGER_HPP
#define ENGINEY_SCENE_MANAGER_HPP

#include <string>
#include <map>

#include <core/UUID.hpp>

namespace EngineY {

    class Scene;

    using SceneMap = std::map<UUID , Scene*>;

    class SceneManager {
        static SceneManager* singleton;

        /// SceneGraph* context = nullptr;
        Scene* current_scene = nullptr;
        SceneMap scenes;

        public:
            static SceneManager* Instance();

            void Cleanup();

            void AddScene(Scene* scene);
            inline Scene* CurrentScene() const { return current_scene; }
    };

} // namespace YE

#endif // !YE_SCENE_MANAGER_HPP
