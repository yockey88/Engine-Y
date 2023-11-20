#include "scene/scene_manager.hpp"

#include <filesystem>

#include "core/defines.hpp"
#include "core/log.hpp"
#include "scene/scene.hpp"

namespace EngineY {

    SceneManager* SceneManager::singleton = nullptr;

    SceneManager* SceneManager::Instance() {
        if (singleton == nullptr) {
            singleton = new SceneManager;
        }
        return singleton;
    }

    void SceneManager::AddScene(Scene* scene) {
        current_scene = scene;
    }
     
    void SceneManager::Cleanup() {
        if (current_scene != nullptr) {
            current_scene->Shutdown();
            ydelete(current_scene);
        }

        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

} // namespace YE
