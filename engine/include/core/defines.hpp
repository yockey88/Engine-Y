#ifndef YE_DEFINES_HPP
#define YE_DEFINES_HPP

#include <cstdint>

#ifdef YE_PLATFORM_WIN
    #define YE_BREAKPOINT __debugbreak()
#elif YE_PLATFORM_LINUX
    #define YE_BREAKPOINT __builtin_debugtrap()
#endif // !YE_PLATFORM_WIN

#ifdef YE_PLATFORM_WIN
    #define YE_EXPORT __declspec(dllexport)
    #define YE_IMPORT __declspec(dllimport)
#else
    #define YE_EXPORT
    #define YE_IMPORT
#endif // !YE_PLATFORM_WIN

#ifdef YE_DEBUG_BUILD
#define ABORT YE_BREAKPOINT;
#else
#define ABORT (void)0
#endif // !YE_DEBUG_BUILD

#ifdef YE_MEMORY_DEBUG
#define ynew new(__FILE__ , __LINE__)
#define ydelete delete
#else
#define ynew new
#define ydelete delete
#endif // !YE_MEMORY_DEBUG

#define ybit(x) (1u << x)

static constexpr uint32_t kTargetFps = 60;
static constexpr uint32_t kTargetFrameTime = 1 / kTargetFps;

#endif // !YE_DEFINES_HPP