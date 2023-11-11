#ifndef YE_LOG_HPP
#define YE_LOG_HPP

#include "core/defines.hpp"
#include "core/logger.hpp"

/** 
 * \todo Create an editor console to log for the user, this is seperate, bc these are dev
 *          logs and the user will need some of this informatino but not all, we should
 *          also handle their errors differently, and have a seperate log for them
*/

#define TRACE_FILE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    }
#define DEBUG_FILE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->debug(__VA_ARGS__); \
    }
#define INFO_FILE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->info(__VA_ARGS__); \
    }
#define WARN_FILE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->warn(__VA_ARGS__); \
    }
#define ERROR_FILE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->error(__VA_ARGS__); \
    }

#define LOG_TRACE(...) \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    }
#define LOG_DEBUG(...) \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->debug(__VA_ARGS__); \
    }
#define LOG_INFO(...) \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->info(__VA_ARGS__); \
    }
#define LOG_WARN(...) \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->warn(__VA_ARGS__); \
    }
#define LOG_ERROR(...) \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->error(__VA_ARGS__); \
    }

#define ENGINE_TRACE(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    } \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    } 
#define ENGINE_DEBUG(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    } \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    } 
#define ENGINE_INFO(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    } \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    } 
#define ENGINE_WARN(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    } \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    } 
#define ENGINE_ERROR(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->trace(__VA_ARGS__); \
    } \
    if (spdlog::get(ENGINE_CONSOLE_NAME)) { \
        spdlog::get(ENGINE_CONSOLE_NAME)->trace(__VA_ARGS__); \
    } 

#define YE_FATAL(...) \
    if (spdlog::get(ENGINE_LOGGER_NAME)) { \
        spdlog::get(ENGINE_LOGGER_NAME)->critical(__VA_ARGS__); \
    } \
    ABORT;

#ifdef YE_DEBUG_BUILD
#define ENTER_FUNCTION_TRACE() \
    { std::string msg = fmt::format("[{}::{}] Enter" , __FUNCTION__ , __LINE__); \
      TRACE_FILE(msg); }
#define EXIT_FUNCTION_TRACE() \
    { std::string msg = fmt::format("[{}::{}] Exit" , __FUNCTION__ , __LINE__); \
      TRACE_FILE(msg); } 

#define ENTER_FUNCTION_TRACE_MSG(...) \
    { std::string msg = fmt::format("[{}::{}] Enter | {}" , __FUNCTION__ , __LINE__ , __VA_ARGS__); \
      TRACE_FILE(msg); }

#define EXIT_FUNCTION_TRACE_MSG(...) \
    { std::string msg = fmt::format("[{}::{}] Exit | {}" , __FUNCTION__ , __LINE__ , __VA_ARGS__); \
      TRACE_FILE(msg); }
#else
#define ENTER_FUNCTION_TRACE() (void)0
#define EXIT_FUNCTION_TRACE() (void)0
#define ENTER_FUNCTION_TRACE_MSG(...) (void)0
#define EXIT_FUNCTION_TRACE_MSG(...) (void)0
#endif

#if 0 // preparing for later (when I can get it to work lmao)
#ifdef YE_DEBUG_BUILD
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