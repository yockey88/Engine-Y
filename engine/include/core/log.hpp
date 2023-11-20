#ifndef ENGINEY_LOG_HPP
#define ENGINEY_LOG_HPP

#include <spdlog/spdlog.h>

#include "core/defines.hpp"
#include "core/logger.hpp"

#define XLOG(level , type , msg) \
    { \
        EngineY::Logger::Instance()->SendLog( \
            EngineY::Logger::LogLevel::level , EngineY::Logger::LogType::type , \
            __FUNCTION__ , std::to_string(__LINE__) , msg \
        ); \
    } \

#define ENGINE_TRACE(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(TRACE , ALL_LOGS , msg); \
    }
#define ENGINE_DEBUG(f , ...)\
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(DEBUG , ALL_LOGS , msg); \
    }
#define ENGINE_INFO(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(INFO , ALL_LOGS , msg); \
    }
#define ENGINE_WARN(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(WARN , ALL_LOGS , msg); \
    }
#define ENGINE_ERROR(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(ERR , ALL_LOGS , msg); \
    }
#define ENGINE_CRITICAL(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(CRITICAL , ALL_LOGS , msg); \
    }
#define ENGINE_FATAL(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(CRITICAL , ALL_LOGS , msg); \
        ENGINE_ABORT; \
    }
// maybe do some more stuff here to give detailed error reporting?
#define ENGINE_ASSERT(x , f , ...) \
    if ((x)) {} else {         \
        ENGINE_FATAL(f , ## __VA_ARGS__); \
    }

#define LOG_TRACE(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(TRACE , CONSOLE , msg); \
    }
#define LOG_DEBUG(f , ...)\
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(DEBUG , CONSOLE , msg); \
    }
#define LOG_INFO(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(INFO , CONSOLE , msg); \
    }
#define LOG_WARN(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(WARN , CONSOLE , msg); \
    }
#define LOG_ERROR(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(ERR , CONSOLE , msg); \
    }
#define LOG_CRITICAL(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(CRITICAL , CONSOLE , msg); \
    }

#define ENGINE_FTRACE(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(TRACE , FILE , msg); \
    }
#define ENGINE_FDEBUG(f , ...)\
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(DEBUG , FILE , msg); \
    }
#define ENGINE_FINFO(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(INFO , FILE , msg); \
    }
#define ENGINE_FWARN(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(WARN , FILE , msg); \
    }
#define ENGINE_FERROR(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(ERR , FILE , msg); \
    }
#define ENGINE_FCRITICAL(f , ...) \
    { \
        std::string msg = fmt::format(fmt::runtime(f) , ## __VA_ARGS__); \
        XLOG(CRITICAL , FILE , msg); \
    }

#endif // !ENGINEY_LOG_HPP
