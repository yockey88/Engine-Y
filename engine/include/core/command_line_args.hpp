#ifndef YE_COMMAND_LINE_ARGS_HPP
#define YE_COMMAND_LINE_ARGS_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <map>
#include <array>

namespace EngineY {

    static constexpr uint32_t kNumShortFlags = 5;
    static constexpr std::array<std::string_view , kNumShortFlags> kShortFlags = {
        "-n" , "-f" , "-p" , "-mp" , "-e"
    };


    static constexpr uint32_t kNumValidFlags = kNumShortFlags * 2;
    static constexpr std::array<std::string_view , kNumValidFlags> kValidFlags = {
        "-n"   , "--project-name" , "-f"   , "--project-file" , "-p"   , "--project-path"  , 
        "-mp"  , "--modules-path" , "-e"   , "--in-editor"
    };

    
    enum CmndLineFlag : uint32_t {
        PROJECT_NAME ,
        PROJECT_FILE , 
        PROJECT_PATH ,
        MODULES_PATH  ,
        IN_EDITOR , 
        INVALID // Number of flags
    };

    struct Argument {
        std::string flag;
        std::string value;
    };

    class CmndLineHandler {
        std::string exe_path;
        std::map<std::string , Argument> args;

        bool args_valid = false;

        bool FlagValid(const std::string& flag) const;
        bool FlagRequiresValue(const std::string& flag) const;
        bool ValidateArgs() const;
        std::string ShortenArg(const std::string& arg) const;

        public:
            CmndLineHandler(int argc , char* argv[]);
            ~CmndLineHandler() {}

            const void DumpArgs() const;
            bool ArgsValid() const;
            const bool FlagExists(const std::string& flag) const;
            const bool FlagExists(CmndLineFlag flag) const;
            const std::string RetrieveValue(const std::string& flag);
            const std::string RetrieveValue(CmndLineFlag flag);
            const Argument RetrieveArgument(const std::string& flag);
            const Argument RetrieveArgument(CmndLineFlag flag);
    };

} // EngineY

#endif // !ENGINEY_COMMAND_LINE_ARGS_HPP
