#ifndef ENGINEY_DEFINES_HPP
#define ENGINEY_DEFINES_HPP

#include <string>
#include <string_view>

#ifdef ENGINEY_WINDOWS
// #define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include <ShlObj.h>
#include <minwindef.h>
    #ifdef ENGINEY_DEBUG 
        #include "platform/windows/win_crash_handler.hpp"
    #endif // !ENGINEY_DEBUG
#else
#endif // !ENGINEY_WINDOWS

#include <cstdint>

#define ENGINEY_VERSION_MAJOR 0
#define ENGINEY_VERSION_MINOR 2
#define ENGINEY_VERSION_PATCH 3

#define ENGINEY_VERSION "0.2.3"

#ifdef ENGINEY_WINDOWS
#define ENGINE_BREAK __debugbreak()
#elif ENGINEY_LINUX
#define ENGINE_BREAK __builtin_trap()
#endif // !ENGINEY_WINDOWS

#ifdef ENGINEY_DEBUG
#define ENGINE_ABORT ENGINE_BREAK
#else
#define ENGINE_ABORT (void)0
#endif // !ENGINEY_DEBUG

#define BIT(x) (1 << x)

#ifdef ENGINEY_MEMORY_DEBUG
///> This is super slow and should only be used when debugging memory leaks
///   or we can make it faster by mapping pointer to its destructor
///   we would have to make sure that each ptr is unique (within scope, i.e, 
///   remove old pointers in case address is reused)
///   We can also find a way to remove indirection somehow, but idk how since memory manager
///   needs to be accessible globally, maybe we can make everything static???

// #include "core/memory/memory_manager.hpp"
// /// when memory debug is on we will have the memory manager tell us when we we forget to delete something
// #define ynew(type , ...) \
//     EngineY::MemoryManager::Instance()->New<type>(__VA_ARGS__) 
// #define tracked_new(type , size) \
//     (type*)EngineY::MemoryManager::Instance()->Allocate(size)
// #define ydelete(obj) \
//     EngineY::MemoryManager::Instance()->Delete(obj)
// #define free_tracker(obj) \
//     EngineY::MemoryManager::Instance()->Deallocate(obj)
// //*/
// #else
#define ynew(type , ...) new type(__VA_ARGS__)
#define tracked_new(size) new byte[size]
#define ydelete(obj) delete obj
#define free_tracker(obj) delete[] (byte*)obj
#endif // !YE_MEMORY_DEBUG


#define scoped_new(type , ...) MemoryManager::Instance()->FrameAllocate<type>(__VA_ARGS__);

#define ybit(x) (1u << x)

static constexpr uint32_t kTargetFps = 120;
static constexpr uint32_t kTargetFrameTime = 1 / kTargetFps;

#include <string>

#include <SDL.h>
#include <glm/glm.hpp>

#endif // !YE_DEFINES_HPP
