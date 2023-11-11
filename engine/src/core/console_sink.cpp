#include "core/console_sink.hpp"

namespace YE {

    void LogMsgToConsoleMsgSink(const spdlog::details::log_msg& msg) {
        ConsoleMessage console_msg;
        switch (msg.level) {
            case spdlog::level::trace:    console_msg.type = ConsoleMessageType::TRACE; break;
            case spdlog::level::debug:    console_msg.type = ConsoleMessageType::DEBUG; break;
            case spdlog::level::info:     console_msg.type = ConsoleMessageType::INFO;  break;
            case spdlog::level::warn:     console_msg.type = ConsoleMessageType::WARN;  break;
            case spdlog::level::err:      console_msg.type = ConsoleMessageType::ERR;   break;
            case spdlog::level::critical: console_msg.type = ConsoleMessageType::ERR;   break;
            default:
                YE_CRITICAL_ASSERTION(false , "Invalid spdlog::level::level_enum value | UNDEFINED BEHAVIOR OR NON-EXHAUSTIVE SWITCH");
                break;
        }

        char buffer[2048];
        if (msg.payload.size() > 2048) {
            memcpy(buffer , msg.payload.data() , 2047);
            buffer[2047] = '\0';
            console_msg.message = std::string{ buffer } + "...";
        } else {
            memcpy(buffer , msg.payload.data() , msg.payload.size());
            buffer[msg.payload.size()] = '\0';
            console_msg.message = std::string{ buffer };
        }

        EngineConsole::SinkMessage(console_msg);
    }

} // namespace YE