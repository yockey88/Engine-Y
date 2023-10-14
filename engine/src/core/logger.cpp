#include "core/logger.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"

#include "log.hpp"
#include "core/defines.hpp"

namespace YE {

    Logger* Logger::singleton = nullptr;

    Logger* Logger::Instance() {
        if (singleton == nullptr)
            singleton = ynew Logger;
        return singleton;
    }

    void Logger::OpenLog() {
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("%^[%l | %c]%$ :: %v");
        console_sink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks1 { console_sink };

        logger = std::make_shared<spdlog::logger>(CONSOLE_LOG_NAME , sinks1.begin() , sinks1.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);

        spdlog::register_logger(logger);
    }

    void Logger::CloseLog() {
        if (singleton != nullptr) ydelete singleton;
    }   

}