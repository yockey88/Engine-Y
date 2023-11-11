#include "core/task_manager.hpp"

#include "log.hpp"
namespace YE {

    TaskManager* TaskManager::singleton = nullptr;
    
    TaskManager* TaskManager::Instance() {
        if (singleton == nullptr) {
            singleton = ynew TaskManager;
        }
        return singleton;
    }

    void TaskManager::DispatchTask(BaseTask* task) {
        ENTER_FUNCTION_TRACE();

        task_queue.push(task);
        task->Dispatch();

        EXIT_FUNCTION_TRACE();
    }

    void TaskManager::FlushTasks() {
        while (!task_queue.empty()) {
            BaseTask* task = task_queue.front();
            task_queue.pop();
            task->WaitFor();
            ydelete task;
        }
    }
    
    void TaskManager::Cleanup() {
        ENTER_FUNCTION_TRACE();

        this->FlushTasks();
        if (singleton != nullptr) ydelete singleton;

        EXIT_FUNCTION_TRACE();
    }

}