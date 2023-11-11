#ifndef YE_LOGGER_HPP
#define YE_LOGGER_HPP

#include <mutex>

#include <spdlog/spdlog.h>
#include <spdlog/pattern_formatter.h>

#include "defines.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define YE_LOG_PATH "logs/ye.log"
#define ENGINE_LOGGER_NAME "EngineYLogger"
#define ENGINE_CONSOLE_NAME "EngineYConsole"

/**
 * Operator overloads for printing glm types
*/
template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion) {
    return os << glm::to_string(quaternion);
}

namespace YE {

    enum class LogLevel {
        TRACE = spdlog::level::trace ,
        DEBUG = spdlog::level::debug ,
        INFO = spdlog::level::info , 
        WARN = spdlog::level::warn , 
        ERR = spdlog::level::err ,
    };

    enum class Target {
        FILE , 
        CONSOLE ,
        BOTH
    };    

    class Logger {

        static Logger* singleton;

        std::shared_ptr<spdlog::logger> logger = nullptr;
        std::shared_ptr<spdlog::logger> console_logger = nullptr;

        Logger() {}
        ~Logger() {}

        Logger(Logger&&) = delete;
        Logger(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;

        public:

            static Logger* Instance();

            void OpenLog();
            void OpenConsole();

            void ChangeLogLevel(LogLevel level , Target target);
            void DumpBacktrace();

            void CloseLog();

    };

}

#endif