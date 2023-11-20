#ifndef ENGINEY_TASK_MANAGER_HPP
#define ENGINEY_TASK_MANAGER_HPP

#include <queue>

#include "core/tasks.hpp"
#include "core/defines.hpp"
#include "core/log.hpp"

namespace EngineY {

    class TaskManager {

        static TaskManager* singleton;

        std::queue<BaseTask*> task_queue;
        
        TaskManager() {}
        ~TaskManager() {}

        TaskManager(TaskManager&&) = delete;
        TaskManager(const TaskManager&) = delete;
        TaskManager& operator=(TaskManager&&) = delete;
        TaskManager& operator=(const TaskManager&) = delete;

        public:

            static TaskManager* Instance();

            void DispatchTask(BaseTask* task);

            // Update this so that when the task is detached it sends a message to 
            //  the right place to alert them that the task is done
            template<typename C , typename... Args>
            void DispatchTask(C&& callable , Args&&... args)  {
                BaseTask* task = new Task(
                    std::forward<C>(callable) , 
                    std::forward<Args>(args)...
                );
                task_queue.push(task);
                task->Dispatch();
            }

            void FlushTasks();

            void Cleanup();
    };

}

#endif // !YE_TASK_MANAGER_HPP
