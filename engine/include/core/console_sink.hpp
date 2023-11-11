#ifndef YE_CONSOLE_SINK_HPP
#define YE_CONSOLE_SINK_HPP

#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>
#include <string>

#include "log.hpp"
#include "log_sinks_base.hpp"
#include "console.hpp"

namespace YE {

    void LogMsgToConsoleMsgSink(const spdlog::details::log_msg& msg);

    class EngineConsoleSink : public spdlog::sinks::callback_sink_mt {
        public:
            EngineConsoleSink() 
                : callback_sink(LogMsgToConsoleMsgSink) {}
    }; 

} // namespace YE

#endif // !YE_CONSOLE_SINK_HPP