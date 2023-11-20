#include "core/task_manager.hpp"

#include "core/log.hpp"
#include "core/memory/memory_manager.hpp"

namespace EngineY {

    TaskManager* TaskManager::singleton = nullptr;
    
    TaskManager* TaskManager::Instance() {
        if (singleton == nullptr) {
            singleton = new TaskManager;
        }
        return singleton;
    }

    void TaskManager::DispatchTask(BaseTask* task) {
        task_queue.push(task);
        task->Dispatch();
    }

    void TaskManager::FlushTasks() {
        while (!task_queue.empty()) {
            BaseTask* task = task_queue.front();
            task_queue.pop();
            task->WaitFor();
            ydelete(task);
        }
    }
    
    void TaskManager::Cleanup() {
        this->FlushTasks();
        if (singleton != nullptr) delete singleton;
    }

}
