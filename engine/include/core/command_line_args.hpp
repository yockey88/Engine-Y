#ifndef YE_COMMAND_LINE_ARGS_HPP
#define YE_COMMAND_LINE_ARGS_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <map>
#include <array>

namespace YE {

    static constexpr uint32_t kNumShortFlags = 9;
    static constexpr std::array<std::string_view , kNumShortFlags> kShortFlags = {
        "-n" , "-f" , "-p" , "-wd" , "-md" , "-pb" , "-mcp" , "-er" , "-fl"
    };


    static constexpr uint32_t kNumValidFlags = 18;
    static constexpr std::array<std::string_view , kNumValidFlags> kValidFlags = {
        "-n"   , "--project-name"      , "-f"   , "--project-file"      , "-p"   , "--project-path" , 
        "-wd"  , "--working-directory" , "-md"  , "--modules-directory" , "-pb"  , "--project-bin"  , 
        "-mcp" , "--mono-config-path"  , "-er"  , "--engine-root"       , "-fl"  , "--from-launcher"
    };

    
    enum CmndLineFlag : uint32_t {
        PROJECT_NAME ,
        PROJECT_FILE , 
        PROJECT_PATH ,
        WORKING_DIR  ,
        MODULES_DIR  ,
        PROJECT_BIN  ,
        MONO_CONFIG_PATH ,
        ENGINE_ROOT ,
        FROM_LAUNCHER ,
        INVALID 
    };

    struct Argument {
        std::string flag;
        std::string value;
    };

    class CmndLineHandler {
        std::string exe_path;
        std::map<std::string , Argument> args;

        bool FlagValid(const std::string& flag) const;
        bool FlagRequiresValue(const std::string& flag) const;
        bool ValidateArgs() const;
        std::string ShortenArg(const std::string& arg) const;

        public:
            CmndLineHandler() {}
            ~CmndLineHandler() {}

            const void DumpArgs() const;
            bool Parse(int argc , char* argv[]);
            const bool FlagExists(const std::string& flag) const;
            const bool FlagExists(CmndLineFlag flag) const;
            const std::string RetrieveValue(const std::string& flag);
            const std::string RetrieveValue(CmndLineFlag flag);
            const Argument RetrieveArgument(const std::string& flag);
            const Argument RetrieveArgument(CmndLineFlag flag);
    };

}

#endif // !YE_COMMAND_LINE_ARGS_HPP
