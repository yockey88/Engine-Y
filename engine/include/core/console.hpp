#ifndef YE_CONSOLE_HPP
#define YE_CONSOLE_HPP

#include <string>
#include <vector>
#include <functional>

#include "defines.hpp"

namespace YE {

    enum ConsoleMessageType : uint16_t {
        TRACE   = ybit(0),
        DEBUG   = ybit(1) ,
        INFO    = ybit(2) , 
        WARN    = ybit(3) , 
        ERR     = ybit(4) , 
        COMMAND = ybit(5) ,
        ALL     = ybit(6)
    };

    struct ConsoleMessage {
        ConsoleMessageType type;
        std::string message;
    };

    using ConsolePanelCallback = std::function<void(const std::vector<void*>&)>; 
    using ConsolePanel = std::pair<std::string , ConsolePanelCallback>;

    class EngineConsole {
        static std::vector<ConsoleMessage> command_history;
        static std::vector<ConsoleMessage> message_history;
        static std::vector<ConsoleMessage> messages;

        uint16_t filter_flags = (uint16_t)ConsoleMessageType::ALL;
        
        static void ExecuteCommand(const ConsoleMessage& cmd);

        public:
            void Initialize();
            void Update();
            void DrawGui(const std::vector<ConsolePanel>& panels = {});
            void Shutdown(); 

            static void SinkMessage(const ConsoleMessage& msg); 
    };

} // namespace YE 

#endif // !YE_CONSOLE_HPP