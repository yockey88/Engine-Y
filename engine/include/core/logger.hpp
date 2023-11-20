#ifndef ENGINEY_LOGGER_HPP
#define ENGINEY_LOGGER_HPP

#include <string>
#include <string_view>

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define ENGINE_CONSOLE_NAME "EngineY-Console"
#define ENGINE_FILE_NAME "EngineY-LogFile"
#define ENGINE_LOGGER_NAME "EngineY-Logger"

#define ENGINE_DEFAULT_LOG_PATH "logs/EngineY-LogFile.log"

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

namespace EngineY {

    class Logger {
        public:
            using spdlog_level = spdlog::level::level_enum;
            enum LogLevel {
                TRACE = spdlog_level::trace ,
                DEBUG = spdlog_level::debug ,
                INFO = spdlog_level::info ,
                WARN = spdlog_level::warn ,
                ERR = spdlog_level::err ,
                CRITICAL = spdlog_level::critical ,
                OFF = spdlog_level::off ,
                NUM_LOG_LEVELS
            };

            enum LogType {
                CONSOLE ,
                FILE ,
                ALL_LOGS
            };

        private:
            static Logger* singleton;

            LogLevel logger_active_log_level = LogLevel::TRACE;
            LogLevel console_active_log_level = LogLevel::TRACE;

            std::shared_ptr<spdlog::logger> logger = nullptr;
            std::shared_ptr<spdlog::logger> file_logger = nullptr;
            std::shared_ptr<spdlog::logger> console_logger = nullptr;

            /// \todo implement file handlers
            void BeforeLogOpen(const std::string& filename) {}
            void AfterLogOpen(const std::string& filename , std::FILE* file) {}
            void BeforeLogClose(const std::string& filename , std::FILE* file) {}
            void AfterLogClose(const std::string& filename) {}

            void RealSendLog(
                std::shared_ptr<spdlog::logger>& logger , LogLevel level , 
                const std::string& msg
            );

            Logger();
            ~Logger();

            Logger (Logger&&) = delete;
            Logger (const Logger&) = delete;
            Logger& operator= (Logger&&) = delete;
            Logger& operator= (const Logger&) = delete;

        public:
            static Logger* Instance();

            void SetLogLevel(LogLevel level , LogType type = ALL_LOGS);

            void TurnOff(LogType type = ALL_LOGS);

            void SendLog(
                LogLevel level , LogType type , 
                const std::string& function , const std::string& line ,
                const std::string& msg
            );

            /// \brief sets to last active log level
            void TurnOn();
    };

} // namespace EngineY

#endif // !ENGINEY_LOGGER_HPP
