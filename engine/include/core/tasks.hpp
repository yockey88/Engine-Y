#ifndef ENGINEY_TASKS_HPP
#define ENGINEY_TASKS_HPP

#include <functional>
#include <thread>
#include <memory>

namespace EngineY {

    class BaseTask {
        public:
            BaseTask() {}
            virtual ~BaseTask() {}
            virtual void Dispatch() = 0;
            virtual void WaitFor() = 0;
    };

    template<typename C , typename... Args>
    class Task : public BaseTask {
        std::function<void()> callable;
        std::unique_ptr<std::thread> thread = nullptr;

        public:
            Task(C&& func , Args... args) 
                : callable([fn = std::forward<C>(func) , ...params = std::forward<Args>(args)]{ 
                        fn(params...);
                    }) {}
            virtual ~Task() override {}

            virtual void Dispatch() override {
                if (thread != nullptr) return;
                thread = std::make_unique<std::thread>(callable);
            }

            virtual void WaitFor() override {
                if (thread == nullptr) return;

                thread->join();
            }
    };

}

#endif // !YE_TASKS_HPP