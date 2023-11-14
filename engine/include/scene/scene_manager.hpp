#ifndef YE_SCENE_MANAGER_HPP
#define YE_SCENE_MANAGER_HPP

#include <string>
#include <map>

#include <core/UUID.hpp>

namespace YE {

    class Scene;

    using SceneMap = std::map<UUID , Scene*>;

    class SceneManager {
        static SceneManager* singleton;

        Scene* current_scene = nullptr;
        SceneMap scenes;

        public:
            static SceneManager* Instance();

            bool LoadProjectFile(const std::string& ys_file);

            void Cleanup();

            inline Scene* CurrentScene() const { return current_scene; }
    };

} // namespace YE

#endif // !YE_SCENE_MANAGER_HPP