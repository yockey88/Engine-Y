#include "rendering/render_thread.hpp"

#include "log.hpp"

namespace YE {

    RenderThread* RenderThread::singleton = nullptr;
    
    RenderThread* RenderThread::Instance() {
        if (singleton == nullptr) {
            singleton = ynew RenderThread;
        }
        return singleton;
    }

    void RenderThread::Launch() {
        // render_queue = std::make_unique<RenderQueue>();
        // renderer = std::make_unique<Renderer>(render_queue->GetQueue());

        // renderer->Start();
        // thread = std::make_unique<std::thread>([this](std::unique_ptr<Renderer>& renderer) {
        //     renderer->Render();
        // } , std::ref(renderer));
    }

    void RenderThread::WaitFor() {
        // render_queue->CloseQueue();
        // renderer->Stop();

        // thread->join();
    }
    
    void RenderThread::Cleanup() {
        if (singleton != nullptr) ydelete singleton;
    }

}