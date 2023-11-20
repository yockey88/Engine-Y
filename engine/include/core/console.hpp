#ifndef ENGINEY_CONSOLE_HPP
#define ENGINEY_CONSOLE_HPP

#include <sstream>
#include <string>
#include <vector>
#include <functional>

#include <imgui/imgui.h>

#include "core/defines.hpp"

namespace EngineY {

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

        static constexpr ImVec4 kWhite  = ImVec4(1.f , 1.f , 1.f , 1.f);
        static constexpr ImVec4 kBlue   = ImVec4(0.f , 0.f , 1.f , 1.f);
        static constexpr ImVec4 kGreen  = ImVec4(0.f , 1.f , 0.f , 1.f);
        static constexpr ImVec4 kYellow = ImVec4(1.f , 1.f , 0.f , 1.f);
        static constexpr ImVec4 kRed    = ImVec4(1.f , 0.f , 0.f , 1.f);

        static constexpr ImVec4 kDefaultConsoleBg = ImVec4(0x0E / 255 , 0x0E / 255 , 0x0E / 255 , 1.f);

        bool is_initialized = false;
        std::string fonts_directory;
        uint16_t filter_flags = (uint16_t)ConsoleMessageType::ALL;

        ImVec4 MsgTypeToColor(ConsoleMessageType type);
        std::string GetMsgHeader(ConsoleMessageType type);

        static constexpr size_t kCmndBufferSize = 512; 
        static char cmnd_buffer[kCmndBufferSize];
        static void ExecuteCommand(const ConsoleMessage& cmd);

        public:
            void Initialize();
            void Update();
            void DrawGui();
            void Shutdown(); 

            static void SinkMessage(const ConsoleMessage& msg); 
    };

} // namespace EngineY 

#endif // !YE_CONSOLE_HPP