#ifndef ENGINEY_MEMORY_MANAGER_HPP
#define ENGINEY_MEMORY_MANAGER_HPP

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

#include "core/logger.hpp"

namespace EngineY {

    class Singleton;

    namespace {

        struct MemoryBlock {
            const void* ptr = nullptr;
            std::function<void()> dtor = nullptr;
            std::size_t size = 0;
            std::string type_name = "";
        };

        struct Tracker {
            std::size_t size = 0;
            const void* ptr = nullptr; 
        };
    
    }  // namespace anonymous

    /// \todo register all singletons here and have it clean all singletons up
    ///     must handle them differently bc of private constructors and destructors
    class MemoryManager {
     
         // static constexpr std::size_t kBlockSize = sizeof(MemoryBlock);
         // static constexpr std::size_t kMaxMemoryBlocks = 1024 * 1024 * 2;
        static constexpr std::size_t kMaxMemorySize = 1024 * 1024 * 100; // 100 MB 

        std::vector<MemoryBlock> blocks{};
        std::vector<Tracker> tracked_memory{};

        uint64_t number_allocs_tracking = 0;

        uint32_t num_allocations = 0;
        uint32_t num_deallocations = 0;
        std::size_t current_offset = 0;

        static MemoryManager* singleton;
        MemoryManager () {}
        ~MemoryManager () {}
        
        MemoryManager (MemoryManager&&) = delete;
        MemoryManager (const MemoryManager&) = delete;
        MemoryManager& operator= (MemoryManager&&) = delete;
        MemoryManager& operator= (const MemoryManager&) = delete;

        public:

            static MemoryManager* Instance();

            void AllocateCoreSystems();
            void DeallocateCoreSystems();

            template <typename T , typename... Args>
            T* New(Args&&... args) {
                MemoryBlock block;
                
                T* ptr = new T(std::forward<Args>(args)...);
                block.ptr = reinterpret_cast<void*>(ptr);
                block.size = sizeof(T);
                block.type_name = typeid(T).name();

                block.dtor = [ptr] {
                    delete ptr;
                };
                blocks.push_back(block);

                ++num_allocations;

                std::stringstream ss;
                ss << "MemoryManager::New |> Type " 
                    << block.type_name 
                    << " : Allocated " 
                    << block.size 
                    << " bytes : Address " 
                    << std::hex << ptr;

                Logger::Instance()->SendLog(
                    Logger::LogLevel::DEBUG , Logger::LogType::FILE ,
                    "EngineY::MemoryManager::New" , std::string{ __LINE__ } , ss.str() 
                );

                return ptr;
            }

            void* Allocate(std::size_t size);
            void Deallocate(void* ptr);
                
            template <typename T>
            void Delete(T* ptr) {
                if (ptr == nullptr) {
                    Logger::Instance()->SendLog(
                        Logger::LogLevel::ERR , Logger::LogType::ALL_LOGS ,
                        "EngineY::MemoryManager::Delete" , std::string{ __LINE__ } , 
                        "Pointer is nullptr"
                    );

                    return;
                }

                for (auto itr = blocks.begin(); itr != blocks.end();) {
                    if (itr->ptr == ptr) {
                        if (itr->dtor != nullptr) {
                            itr->dtor();
                            ++num_deallocations;
                            std::stringstream ss;
                            ss << "MemoryManager::Delete |> Type " 
                                << itr->type_name 
                                << " : Deallocated " 
                                << itr->size 
                                << " bytes : Address " 
                                << std::hex << ptr;
                            Logger::Instance()->SendLog(
                                Logger::LogLevel::DEBUG , Logger::LogType::FILE ,
                                "EngineY::MemoryManager::Delete" , std::string{ __LINE__ } , ss.str()
                            );
                        } else {
                            Logger::Instance()->SendLog(
                                Logger::LogLevel::ERR , Logger::LogType::ALL_LOGS , 
                                "EngineY::MemoryManager::Delete" , std::to_string(__LINE__)  , 
                                "Object had no destructor to call"
                            );
                        }
                        itr->ptr = nullptr;
                        itr = blocks.erase(itr);
                        return;
                    } else {
                        ++itr;
                    }
                }
            }

            void Cleanup();

            inline const std::size_t NumBlocks() const { return blocks.size(); }
    };

} // namespace EngineY

#endif // !ENGINEY_MEMORY_MANAGER_HPP
