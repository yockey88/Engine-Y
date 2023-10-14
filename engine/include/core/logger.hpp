#ifndef YE_LOGGER_HPP
#define YE_LOGGER_HPP

#include <mutex>

#include "spdlog/spdlog.h"

#include "defines.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#define YE_LOG_PATH "logs/ye.log"
#define CONSOLE_LOG_NAME "YEConsoleLogger"

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
        TRACE ,
        DEBUG ,
        INFO , 
        WARN , 
        ERR , 
        FATAL
    };

    class Logger {

        static Logger* singleton;

        std::shared_ptr<spdlog::logger> logger = nullptr;
        /// \todo add file logger

        Logger() {}
        ~Logger() {}

        Logger(Logger&&) = delete;
        Logger(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;

        public:

            static Logger* Instance();

            void OpenLog();

            template<typename... Args>
            void Log(LogLevel lvl , const std::string& fmt , Args... args) {}

            void CloseLog();

    };

}

#endif