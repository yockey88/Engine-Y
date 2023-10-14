#ifndef YE_TASK_MANAGER_HPP
#define YE_TASK_MANAGER_HPP

#include <queue>

#include "tasks.hpp"
#include "log.hpp"

namespace YE {

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

        template<typename C , typename... Args>
            void DispatchTask(C&& callable , Args&&... args)  {
                BaseTask* task = ynew Task(
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