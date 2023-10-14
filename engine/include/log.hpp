#ifndef YE_LOG_HPP
#define YE_LOG_HPP

#include "core/defines.hpp"
#include "core/logger.hpp"

/** 
 * \todo Create an editor console to log for the user, this is seperate, bc these are dev
 *          logs and the user will need some of this informatino but not all, we should
 *          also handle their errors differently, and have a seperate log for them
*/

#ifdef YE_DEBUG_BUILD
#define YE_TRACE(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->trace(__VA_ARGS__); \
    } 
#define YE_DEBUG(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->debug(__VA_ARGS__); \
    } 
#define YE_INFO(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->info(__VA_ARGS__); \
    }
#define YE_WARN(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->warn(__VA_ARGS__); \
    } 
#define YE_ERROR(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->error(__VA_ARGS__); \
    } 
#define YE_FATAL(...) \
    if (spdlog::get(CONSOLE_LOG_NAME)) { \
        spdlog::get(CONSOLE_LOG_NAME)->critical(__VA_ARGS__); \
    } \
    ABORT;
#define XCRITICAL_ASSERT(x , ...) \
    if ((x)) {} else { \
        if (spdlog::get(CONSOLE_LOG_NAME)) { \
            spdlog::get(CONSOLE_LOG_NAME)->error("[Assertion Failed]"); \
        } \
        if (spdlog::get(CONSOLE_LOG_NAME)) { \
            spdlog::get(CONSOLE_LOG_NAME)->critical(__VA_ARGS__); \
        } \
        ABORT; \
    }
#define YE_CRITICAL_ASSERTION(x , ...) XCRITICAL_ASSERT(x , __VA_ARGS__)
#else
#define YE_TRACE(...) (void)0
#define YE_DEBUG(...) (void)0
#define YE_INFO(...) (void)0
#define YE_WARN(...) (void)0
#define YE_ERROR(...) (void)0
#define YE_FATAL(...) (void)0
#define XCRITICAL_ASSERT(void)0
#define YE_CRITICAL_ASSERTION(x , ...) (void)0
#endif

#if 0 // preparing for later (when I can get it to work lmao)
#ifdef YE_DEBUG
    #define YE_PROFILING_ENABLED 1
#else 
    #define YE_PROFILING_ENABLED 0
#endif

#if YE_PROFILING_ENABLED
    #include <optick/optick.h>

#define YE_PROFILE_START()        OPTICK_START_CAPTURE()
#define YE_PROFILE_STOP()         OPTICK_STOP_CAPTURE()
#define YE_SAVE_PROFILE(filename) OPTICK_SAVE_CAPTURE(filename)

#define YE_PROFILE_FRAME(name)     OPTICK_FRAME(name)
#define YE_PROFILE_SCOPE(...)      OPTICK_EVENT(__VA_ARGS__)
#define YE_PROFILE_SCOPE_DYNAMIC() OPTICK_EVENT_DYNAMIC()
#define YE_PROFILE_CATEGORY(name)  OPTICK_CATEGORY(__VA_ARGS__)
#define YE_PROFILE_THREAD(name)    OPTICK_THREAD(__VA_ARGS__)
#define YE_PROFILE_TAG(name , ...) OPTICK_TAG(name , __VA_ARGS__)

#define YE_SET_PROFILE_ALLOCATOR(allocator)       OPTICK_SET_ALLOCATOR(allocator)
#define YE_SET_PROFILE_CALLBACK(profile_callback) OPTICK_SET_EVENT_CALLBACK(profile_callback)

#define YE_PROFILE_SHUTDOWN() OPTICK_SHUTDOWN()

#define YE_INIT_VULKAN_PROFILING(devices , physical_dev , cmd_qs , cmd_qs_fam , num_cmd_qs) \
            OPTICK_GPU_INIT_VULKAN(devices , physical_dev , cmd_qs , cmd_qs_fam , num_cmd_qs)
#define YE_GPU_PROFILE_CONTEXT(...) OPTICK_GPU_CONTEXT(__VA_ARGS__)

#define YE_GPU_PROFILE_EVENT(name) OPTICK_GPU_EVENT(name)
#define YE_GPU_FLIP(swapchain)     OPTICK_GPU_FLIP(swapchain)
#else 
#define YE_PROFILE_START()                        (void)0
#define YE_PROFILE_STOP()                         (void)0
#define YE_SAVE_PROFILE(filename)                 (void)0
#define YE_PROFILE_SCOPE(...)                     (void)0
#define YE_PROFILE_SCOPE_DYNAMIC()                (void)0
#define YE_PROFILE_CATEGORY(name)                 (void)0
#define YE_PROFILE_THREAD(name)                   (void)0
#define YE_PROFILE_TAG(name , ...)                (void)0
#define YE_SET_PROFILE_ALLOCATOR(allocator)       (void)0
#define YE_SET_PROFILE_CALLBACK(profile_callback) (void)0
#define YE_PROFILE_SHUTDOWN()                     (void)0
#define YE_INIT_VULKAN_PROFILING(devices , physical_dev , cmd_qs , cmd_qs_fam , num_cmd_qs) (void)0
#define YE_GPU_PROFILE_CONTEXT(...) (void)0
#define YE_GPU_PROFILE_EVENT(name) (void)0
#define YE_GPU_FLIP(swapchain)     (void)0
#endif
#endif

#endif // !YE_LOG_HPP