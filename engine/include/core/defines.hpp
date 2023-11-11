#ifndef YE_DEFINES_HPP
#define YE_DEFINES_HPP

#include <cstdint>

#ifdef YE_PLATFORM_WIN
    #define YE_BREAKPOINT __debugbreak()
#elif YE_PLATFORM_LINUX
    #define YE_BREAKPOINT __builtin_debugtrap()
#endif // !YE_PLATFORM_WIN

#ifdef YE_PLATFORM_WIN
#endif // !YE_PLATFORM_WIN

#ifdef YE_DEBUG_BUILD
#define ABORT YE_BREAKPOINT;

#define XCRITICAL_ASSERT(x , ...) \
    if ((x)) {} else { \
        if (spdlog::get(ENGINE_LOGGER_NAME)) { \
            spdlog::get(ENGINE_LOGGER_NAME)->error("[Assertion Failed]"); \
            spdlog::get(ENGINE_LOGGER_NAME)->critical(__VA_ARGS__); \
        } \
        ABORT; \
    }
#define YE_CRITICAL_ASSERTION(x , ...) XCRITICAL_ASSERT(x , __VA_ARGS__)
#else
#define ABORT (void)0

#define XCRITICAL_ASSERT(...) (void)0
#define YE_CRITICAL_ASSERTION(x , ...) (void)0
#endif // !YE_DEBUG_BUILD

/// \todo redirect use of ynew and ydelete to custom memory manager

#ifdef YE_MEMORY_DEBUG
#define ynew new(__FILE__ , __LINE__)
#define ydelete delete
#else
#define ynew new
#define ydelete delete
#endif // !YE_MEMORY_DEBUG

#define ybit(x) (1u << x)

static constexpr uint32_t kTargetFps = 120;
static constexpr uint32_t kTargetFrameTime = 1 / kTargetFps;

#include <string>

#include <SDL.h>
#include <glm/glm.hpp>

namespace YE {
    
    struct WindowConfig {
        glm::ivec2 position = glm::ivec2(SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED);
        glm::ivec2 min_size = glm::ivec2(200 , 200);
        glm::ivec2 size = glm::ivec2(800 , 600);
        glm::vec4 clear_color = glm::vec4(0.2f , 0.3f , 0.3f , 1.0f);
        uint32_t flags = 0;
        glm::ivec4 gl_color_bits = glm::ivec4(8 , 8 , 8 , 8);
        uint32_t gl_stencil_size = 8;
        uint32_t gl_multisample_buffers = 1;
        uint32_t gl_multisample_samples = 16;
        std::string title = "Engine Y";
        
        bool fullscreen = false;
        bool vsync = false;
        bool rendering_to_screen = true;
        bool accelerated_visual = true;
    };

    struct EngineConfig {
        WindowConfig window_config;

        std::string project_name;
        std::string project_file;
        std::string project_path;
        std::string working_directory;
        std::string modules_directory;
        std::string project_bin;
        std::string mono_config_path;
        std::string engine_root;

        bool use_project_file = false;
        
        private: // simply for things the engine needs to set up that should never be overridden
            std::string mono_dll_path;
            
            friend class Filesystem;
            friend class Engine;
    };

} // namespace YE

#endif // !YE_DEFINES_HPP