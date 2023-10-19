#include "core/window_manager.hpp"

#include <algorithm>

#include "log.hpp"

namespace YE {

    WindowManager* WindowManager::singleton = nullptr;

    WindowManager* WindowManager::Instance() {
        if (singleton == nullptr) {
            singleton = ynew WindowManager;
        }
        return singleton;
    }

    void WindowManager::Initialize(Gui* gui) { 
        YE_CRITICAL_ASSERTION(gui != nullptr , "Attempting to initialize window manager with null GUI");
        gui_handle = gui; 
    }

    Window* WindowManager::OpenWindow(const WindowConfig& config) {
        std::string win_title = config.title;
        Window* window = ynew Window(config);
        window->Open();
        std::replace(win_title.begin() , win_title.end() , ' ' , '-');

        UUID32 id = Hash::FNV32(win_title);
        if (windows.find(id) != windows.end()) {
            YE_WARN("Failed to push window :: [{0}] | Name already exists" , win_title);
            ydelete window;
            return nullptr;
        }
        windows[id] = window;

        if (gui_handle->WindowCount() == 0) {
            gui_handle->Initialize(window);
        } else {
            gui_handle->PushWindow(window , id);
        }

        window->Clear();
        window->SwapBuffers();

        return window;
    }
    
    Window* WindowManager::WindowByName(const std::string& name) {
        return nullptr;
    }

    Window* WindowManager::WindowByID(UUID32 id) {
        return nullptr;
    }
     
    void WindowManager::ClearWindows() {
        if (windows.size() == 0) return;

        for (auto& window : windows) {
            window.second->Clear();
        }
    }
    
    void WindowManager::SwapWindowBuffers() {
        if (windows.size() == 0) return;

        for (auto& window : windows) {
            window.second->SwapBuffers();
        }
    }

    void WindowManager::CloseWindow(const std::string& name) {

    }
    
    void WindowManager::CloseWindows() {
        if (windows.size() == 0) return;

        for (auto& window : windows) {
            window.second->Close();
            ydelete window.second;
        }
        windows.clear();
    }

    void WindowManager::Cleanup() {
        CloseWindows();
        
        if (singleton != nullptr) ydelete singleton;
        singleton = nullptr;
    }

}