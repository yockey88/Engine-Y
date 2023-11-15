#include "scene/scene_manager.hpp"

#include <filesystem>

#include "log.hpp"
#include "scene/scene.hpp"

namespace YE {

    SceneManager* SceneManager::singleton = nullptr;

    SceneManager* SceneManager::Instance() {
        if (singleton == nullptr) {
            singleton = ynew SceneManager;
        }
        return singleton;
    }

    void SceneManager::AddScene(Scene* scene) {
        current_scene = scene;
    }
     
    void SceneManager::Cleanup() {
        if (current_scene != nullptr) {
            current_scene->Shutdown();
            ydelete current_scene;
        }

        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

} // namespace YE
