#ifndef YE_WINDOW_MANAGER_HPP
#define YE_WINDOW_MANAGER_HPP

#include <map>

#include "window.hpp"
#include "core/defines.hpp"
#include "core/hash.hpp"
#include "core/UUID.hpp"
#include "rendering/gui.hpp"

namespace YE {

    using WindowMap = std::map<UUID32 , Window*>;

    class WindowManager {
        static WindowManager* singleton;

        WindowMap windows;

        Gui* gui_handle = nullptr;

        WindowManager() {}
        ~WindowManager() {}

        WindowManager(WindowManager&&) = delete;
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(WindowManager&&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        public:
            static WindowManager* Instance();

            void Initialize(Gui* gui);

            Window* OpenWindow(const WindowConfig& config);
            Window* WindowByName(const std::string& name);
            Window* WindowByID(UUID32 id);

            void ClearWindows();
            void SwapWindowBuffers();

            void CloseWindow(const std::string& name);

            void CloseWindows();

            void Cleanup();

            inline uint32_t WindowCount() const { return windows.size(); }
    };

}

#endif // !YE_WINDOW_MANAGER_HPP