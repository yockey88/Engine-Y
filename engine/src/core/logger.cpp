#include "core/logger.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>
#include <magic_enum/magic_enum.hpp>

#include "log.hpp"
#include "core/defines.hpp"
#include "core/console_sink.hpp"

namespace YE {

    void BeforeLogOpen(const std::string& filename) {}
    void AfterLogOpen(const std::string& filename , std::FILE* file) {}
    void BeforeLogClose(const std::string& filename , std::FILE* file) {}
    void AfterLogClose(const std::string& filename) {}

    Logger* Logger::singleton = nullptr;

    Logger* Logger::Instance() {
        if (singleton == nullptr)
            singleton = ynew Logger;
        return singleton;
    }

    void Logger::OpenLog() {
#ifdef YE_DEBUG_BUILD
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); 
        console_sink->set_pattern("%^[%l | %c]%$ :: %v");
        console_sink->set_level(spdlog::level::trace);
#endif

        spdlog::file_event_handlers file_handler;
        file_handler.before_open = BeforeLogOpen;
        file_handler.after_open = AfterLogOpen;
        file_handler.before_close = BeforeLogClose;
        file_handler.after_close = AfterLogClose;
    
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink = 
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(YE_LOG_PATH , true);
#ifdef YE_DEBUG_BUILD 
        file_sink->set_level(spdlog::level::trace);
#elif YE_RELEASE_BUILD
        file_sink->set_level(spdlog::level::warn);
#else
        YE_CRITICAL_ASSERTION(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
#endif
        file_sink->set_pattern("%^[%c]-[%l]%$ :: %v < %t >");

        std::vector<spdlog::sink_ptr> sinks{
            file_sink , 
#ifdef YE_DEBUG_BUILD
            console_sink 
#endif
};

        logger = std::make_shared<spdlog::logger>(ENGINE_LOGGER_NAME , sinks.begin() , sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);

        spdlog::register_logger(logger);
    }
    
    void Logger::OpenConsole() {
        ENTER_FUNCTION_TRACE();

        std::shared_ptr<EngineConsoleSink> console_sink = std::make_shared<EngineConsoleSink>();
        console_sink->set_pattern("%^[%l]-[%c]%$ %v");
        console_sink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks{ console_sink };

        console_logger = std::make_shared<spdlog::logger>(ENGINE_CONSOLE_NAME , sinks.begin() , sinks.end());
        console_logger->set_level(spdlog::level::trace);
        console_logger->flush_on(spdlog::level::trace);
        console_logger->enable_backtrace(64);

        spdlog::register_logger(console_logger);

        EXIT_FUNCTION_TRACE();
    }

    void Logger::ChangeLogLevel(LogLevel level , Target target) {
        ENTER_FUNCTION_TRACE_MSG(magic_enum::enum_name(level) , magic_enum::enum_name(target));

        switch (target) {
            case Target::FILE:
                logger->set_level(static_cast<spdlog::level::level_enum>(level));
                logger->flush_on(static_cast<spdlog::level::level_enum>(level));
                break;
            case Target::CONSOLE:
                console_logger->set_level(static_cast<spdlog::level::level_enum>(level));
                console_logger->flush_on(static_cast<spdlog::level::level_enum>(level));
                break;
            case Target::BOTH:
                logger->set_level(static_cast<spdlog::level::level_enum>(level));
                logger->flush_on(static_cast<spdlog::level::level_enum>(level));
                console_logger->set_level(static_cast<spdlog::level::level_enum>(level));
                console_logger->flush_on(static_cast<spdlog::level::level_enum>(level));
                break;
            default:
                YE_CRITICAL_ASSERTION(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
        }

        EXIT_FUNCTION_TRACE();
    }
    
    void Logger::DumpBacktrace() {
        ENTER_FUNCTION_TRACE();

        console_logger->dump_backtrace();

        EXIT_FUNCTION_TRACE();
    }

    void Logger::CloseLog() {
        if (singleton != nullptr) ydelete singleton;
    }   

}