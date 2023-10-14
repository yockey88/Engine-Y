#ifndef YE_RENDER_THREAD_HPP
#define YE_RENDER_THREAD_HPP

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "render_commands.hpp"
#include "renderer.hpp"

namespace YE {

    class RenderThread {

        static RenderThread* singleton;

        // std::unique_ptr<Renderer> renderer = nullptr;
        std::unique_ptr<RenderQueue> render_queue = nullptr;

        std::unique_ptr<std::thread> thread = nullptr;

        std::mutex shutdown_mutex;
        std::condition_variable shutdown_condition;
        
        RenderThread() {}
        ~RenderThread() {}
        
        RenderThread(RenderThread&&) = delete;
        RenderThread(const RenderThread&) = delete;
        RenderThread& operator=(RenderThread&&) = delete;
        RenderThread& operator=(const RenderThread&) = delete;

        public:

            static RenderThread* Instance();

            // inline std::unique_ptr<Renderer>& GetRenderer() { return renderer; }
            inline std::unique_ptr<RenderQueue>& GetQueue() { return render_queue; }

            void Launch();
            void WaitFor();
            void Cleanup();
    };

}

#endif // !YE_RENDER_THREAD_HPP