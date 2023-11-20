#include "core/logger.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>
#include <magic_enum/magic_enum.hpp>

#include "core/logger.hpp"

#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "core/defines.hpp"
#include "core/console_sink.hpp"
#include "core/memory/memory_manager.hpp"

namespace EngineY {

    Logger* Logger::singleton = nullptr;

    void Logger::RealSendLog(
        std::shared_ptr<spdlog::logger>& log , LogLevel level , 
        const std::string& msg
    ) {
        switch (level) {
            case LogLevel::TRACE:    log->trace(msg); break;
            case LogLevel::DEBUG:    log->debug(msg); break;
            case LogLevel::INFO:     log->info(msg); break;
            case LogLevel::WARN:     log->warn(msg); break;
            case LogLevel::ERR:      log->error(msg); break;
            case LogLevel::CRITICAL: log->critical(msg); break;
            default:
                logger->critical("UNDEFINED BEHAVIOR | NON-EXHAUSTIVE SWITCH");
                ENGINE_ABORT;
                break;
        }
    }
    
    Logger::Logger() {
        /**
         * General purpose sinks
         */
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); 
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("%^[%c]-[%l]%$ %v");

        // spdlog::file_event_handlers file_handler;
        // file_handler.before_open = BeforeLogOpen;
        // file_handler.after_open = AfterLogOpen;
        // file_handler.before_close = BeforeLogClose;
        // file_handler.after_close = AfterLogClose;
    
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink = 
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(ENGINE_DEFAULT_LOG_PATH , true);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("%^[%c]-[%l]%$ %v < %t >");

        std::vector<spdlog::sink_ptr> file_sinks{ file_sink };
        std::vector<spdlog::sink_ptr> console_sinks{ console_sink };
        std::vector<spdlog::sink_ptr> sinks{ file_sink , console_sink };

        logger = std::make_shared<spdlog::logger>(ENGINE_LOGGER_NAME , sinks.begin() , sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);

        spdlog::register_logger(logger);
        /**
         * End general purpose sinks
         */
       
        
        /**
         * Specific use sinks
         */
        // File sink
        file_logger = std::make_shared<spdlog::logger>(ENGINE_FILE_NAME , file_sinks.begin() , file_sinks.end());
        file_logger->set_level(spdlog::level::trace);
        file_logger->flush_on(spdlog::level::trace);

        spdlog::register_logger(file_logger);

        // Console sink
        std::shared_ptr<EngineConsoleSink> engine_console = std::make_shared<EngineConsoleSink>();
        engine_console->set_pattern("%^[%l]%$ %v");
        engine_console->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> engine_console_sinks{ engine_console };

        console_logger = std::make_shared<spdlog::logger>(ENGINE_CONSOLE_NAME , engine_console_sinks.begin() , engine_console_sinks.end());
        console_logger->set_level(spdlog::level::trace);
        console_logger->flush_on(spdlog::level::trace);
        console_logger->enable_backtrace(64);

        spdlog::register_logger(console_logger);
        /**
         * End specific use sinks
         */

        logger_active_log_level = LogLevel::TRACE;
        console_active_log_level = LogLevel::TRACE;
    }

    Logger::~Logger() {
        spdlog::drop_all();
    }

    Logger* Logger::Instance() {
        if (singleton == nullptr) {
            singleton = new Logger;
        }
        return singleton;
    }
    
    void Logger::SetLogLevel(LogLevel level , LogType type) {
        if (level < 0 || level >= NUM_LOG_LEVELS) {
            logger->critical("UNDEFINED BEHAVIOR | UNKNOWN LOG LEVEL");
            ENGINE_ABORT;
            return;
        }

        if (type < 0 || type > ALL_LOGS) {
            logger->critical("UNDEFINED BEHAVIOR | UNKNOWN LOG TYPE");
            ENGINE_ABORT;
            return;
        }

        if (type == ALL_LOGS) {
            logger->set_level(static_cast<spdlog_level>(level));
            logger->flush_on(static_cast<spdlog_level>(level));

            logger_active_log_level = level;

            console_logger->set_level(static_cast<spdlog_level>(level));
            console_logger->flush_on(static_cast<spdlog_level>(level));

            console_active_log_level = level;

            return;
        }

        switch (type) {
            case CONSOLE:
                console_logger->set_level(static_cast<spdlog_level>(level));
                console_logger->flush_on(static_cast<spdlog_level>(level));

                console_active_log_level = level;
            break;
            case FILE:
                logger->set_level(static_cast<spdlog_level>(level));
                logger->flush_on(static_cast<spdlog_level>(level));

                logger_active_log_level = level;
            break;
            default:
                logger->critical("UNDEFINED BEHAVIOR | NON-EXHAUSTIVE SWITCH");
                ENGINE_ABORT;
                break;
        }
    }

    void Logger::TurnOff(LogType type) {
        if (type < 0 || type > ALL_LOGS) {
            logger->critical("UNDEFINED BEHAVIOR | UNKNOWN LOG TYPE");
            ENGINE_ABORT;   
            return;
        }
        
        if (type == ALL_LOGS) {
            logger->set_level(spdlog::level::off);
            logger->flush_on(spdlog::level::off);
            return;
        }

        switch (type) {
            case CONSOLE:
                console_logger->set_level(spdlog::level::off);
                console_logger->flush_on(spdlog::level::off);
            break;
            case FILE:
                logger->set_level(spdlog::level::off);
                logger->flush_on(spdlog::level::off);
            break;
            default:
                logger->critical("UNDEFINED BEHAVIOR | NON-EXHAUSTIVE SWITCH");
                ENGINE_ABORT;
                break;
        }
    }

    void Logger::TurnOn() {
        logger->set_level(static_cast<spdlog_level>(logger_active_log_level));
        logger->flush_on(static_cast<spdlog_level>(logger_active_log_level));

        console_logger->set_level(static_cast<spdlog_level>(console_active_log_level));
        console_logger->flush_on(static_cast<spdlog_level>(console_active_log_level));
    }
    
    void Logger::SendLog(
        LogLevel level , LogType type , 
        const std::string& function , const std::string& line ,
        const std::string& msg
    ) {
        if (level < 0 || level >= NUM_LOG_LEVELS) {
            logger->critical("UNDEFINED BEHAVIOR | UNKNOWN LOG LEVEL");
            ENGINE_ABORT;
            return;
        }

        if (type < 0 || type > ALL_LOGS) {
            logger->critical("UNDEFINED BEHAVIOR | UNKNOWN LOG TYPE");
            ENGINE_ABORT;
            return;
        }
        
        const std::string full_msg = "[" + function+ ":" + line + "] :: " + msg;
        switch (type) {
            case CONSOLE:  RealSendLog(console_logger , level , full_msg); break;
            case FILE:     RealSendLog(file_logger , level , full_msg); break;
            case ALL_LOGS: RealSendLog(logger , level , full_msg); break;
            default:
                logger->critical("UNDEFINED BEHAVIOR | NON-EXHAUSTIVE SWITCH");
                ENGINE_ABORT;
                break;
        }
    }    

} // namespace EngineY
