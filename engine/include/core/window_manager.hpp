#ifndef YE_WINDOW_MANAGER_HPP
#define YE_WINDOW_MANAGER_HPP

namespace YE {

    class WindowManager {
        static WindowManager* singleton;

        WindowManager() {}
        ~WindowManager() {}

        WindowManager(WindowManager&&) = delete;
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(WindowManager&&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        public:
            static WindowManager* Instance();

            void Cleanup();
    };

}

#endif // !YE_WINDOW_MANAGER_HPP