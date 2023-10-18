#include "core/window_manager.hpp"

namespace YE {

    WindowManager* WindowManager::singleton = nullptr;

    WindowManager* WindowManager::Instance() {
        if (singleton == nullptr) {
            singleton = new WindowManager;
        }
        return singleton;
    }

    void WindowManager::OpenWindow(const WindowConfig& config) {
        Window* window = new Window(config);
        window->Open();
    }

    void WindowManager::Cleanup() {
        delete singleton;
        singleton = nullptr;
    }

}