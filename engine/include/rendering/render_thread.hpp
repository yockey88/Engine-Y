#ifndef ENGINEY_RENDER_THREAD_HPP
#define ENGINEY_RENDER_THREAD_HPP

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "core/thread.hpp"
#include "core/engine_message.hpp"
#include "core/message_passer.hpp"
#include "rendering/render_commands.hpp"
#include "rendering/renderer.hpp"

namespace EngineY {

    class App;

    class RenderThread {

        static RenderThread* singleton;

        std::unique_ptr<Renderer> renderer = nullptr;
        Thread thread;

        time::FrameRateEnforcer<kTargetFps> frame_rate;
       
        bool running = false;
        bool active = false;

        uint32_t RenderMain();

        std::queue<std::unique_ptr<EngineMessage>> messages_to_send;

        MessageReciever* msg_receiver = nullptr;
        MessageSender* msg_sender = nullptr;

        RenderThread() {}
        ~RenderThread() {}
        
        RenderThread(RenderThread&&) = delete;
        RenderThread(const RenderThread&) = delete;
        RenderThread& operator=(RenderThread&&) = delete;
        RenderThread& operator=(const RenderThread&) = delete;
            
        public:
            static RenderThread* Instance();


            void Launch(
                App* app , WindowConfig& config , 
                MessageQueue& recv_queue , MessageQueue& send_queue
            );
            void GetMessageReceiver();
            void Cleanup();
    };

}

#endif // !YE_RENDER_THREAD_HPP
