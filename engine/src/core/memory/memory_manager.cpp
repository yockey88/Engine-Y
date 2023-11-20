#include "core/memory/memory_manager.hpp"

#include "core/log.hpp"
#include "core/logger.hpp"
#include "core/resource_handler.hpp"
#include "event/event_manager.hpp"
#include "core/task_manager.hpp"
#include "rendering/renderer.hpp"
#include "scripting/script_engine.hpp"
#include "physics/physics_engine.hpp"
#include "scene/scene_manager.hpp"

namespace EngineY {

    MemoryManager* MemoryManager::singleton = nullptr;

    MemoryManager* MemoryManager::Instance() {
        if (singleton == nullptr)
            singleton = new MemoryManager;
        return singleton;
    }

    void MemoryManager::AllocateCoreSystems() {
        TaskManager::Instance();
        ResourceHandler::Instance();
        EventManager::Instance();
        Renderer::Instance(); 
        ScriptEngine::Instance();
        PhysicsEngine::Instance();
        SceneManager::Instance();
    }
    
    void MemoryManager::DeallocateCoreSystems() {
        SceneManager::Instance()->Cleanup();
        PhysicsEngine::Instance()->Cleanup();
        ResourceHandler::Instance()->Cleanup();
        EventManager::Instance()->Cleanup();
        TaskManager::Instance()->Cleanup();
        Renderer::Instance()->Cleanup();
        ScriptEngine::Instance()->Cleanup();

        Cleanup();
    }
    
    void* MemoryManager::Allocate(std::size_t size) {
        Tracker tracker;   
        tracker.size = size;

        void* ptr = new uint8_t[size];
        tracker.ptr = ptr;

        tracked_memory.push_back(tracker);

        ++num_allocations;
        ++number_allocs_tracking;

        ENGINE_FDEBUG("MemoryManager::Allocate |> Allocated {0} bytes : Address {1}" , size , ptr);

        return ptr;
    }
    
    void MemoryManager::Deallocate(void* ptr) {
        if  (ptr == nullptr) {
            ENGINE_ERROR("MemoryManager::Deallocate |> Pointer is null");
            return;
        }

        for (auto it = tracked_memory.begin(); it != tracked_memory.end(); ++it) {
            if (it->ptr == ptr) {
                delete[] (uint8_t*)ptr;
                tracked_memory.erase(it);
                ++num_deallocations;
                --number_allocs_tracking;
                ENGINE_FDEBUG("MemoryManager::Allocate |> Allocated {0} bytes : Address {1}" , it->size , ptr);
                return;
            }
        }

        ENGINE_ERROR("MemoryManager::Deallocate |> Pointer not found");
        ENGINE_ERROR("MemoryManager::Deallocate |> Pointer address: {0:#16x}" , ptr);
    }
    
    void MemoryManager::Cleanup() {

        ENGINE_DEBUG("MemoryManager |> num_allocations: {0}" , num_allocations);
        ENGINE_DEBUG("MemoryManager |> num_deallocations: {0}" , num_deallocations);

        if (blocks.size() > 0) {
            ENGINE_WARN("Memory leaks detected! Failed to delete [{0}] objects" , blocks.size());
            for (auto& block : blocks) {
                ENGINE_DEBUG(
                    "Block Type: {0} | Block size: {1} | Block Address {2}" , 
                    block.type_name , block.size , block.ptr
                );
                block.dtor();
                block.ptr = nullptr;
            }
            blocks.clear();
        }

        if (tracked_memory.size() > 0) {
            ENGINE_WARN("Memory leaks detected! Failed to delete [{0}] objects" , tracked_memory.size());
            for (auto& tracker : tracked_memory) {
                ENGINE_DEBUG(
                    "Tracker size: {0} | Tracker Address {1:#16x}" , 
                    tracker.size , tracker.ptr
                );
                delete[] (byte*)tracker.ptr;
                tracker.ptr = nullptr;
            }
            tracked_memory.clear();
        }
        
        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

} // namespace EngineY
