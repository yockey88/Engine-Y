#ifndef YE_COMMAND_LINE_ARGS_HPP
#define YE_COMMAND_LINE_ARGS_HPP

#include <string>
#include <string_view>
#include <map>
#include <array>

namespace YE {

    static constexpr uint32_t kNumShortFlags = 10;
    static constexpr std::array<std::string_view , kNumShortFlags> kShortFlags = {
        "-n" , "-f" , "-p" , "-wd" , "-md" , "-pmd" , "-bc" , "-mdp" , "-mcp" , "-er"
    };


    static constexpr uint32_t kNumValidFlags = 18;
    static constexpr std::array<std::string_view , kNumValidFlags> kValidFlags = {
        "-n"   , "--project-name"      , "-f"   , "--project-file"      , "-p"   , "--project-path" , 
        "-wd"  , "--working-directory" , "-md"  , "--modules-directory" , "-pb"  , "--project-bin"  , 
        "-mdp" , "--mono-dll-path"     , "-mcp" , "--mono-config-path"  , "-er"  , "--engine-root"  
    };

    
    enum CmndLineFlag : uint32_t {
        PROJECT_FILE = 0 , 
        PROJECT_PATH  ,
        WORKING_DIR ,
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

            bool Parse(int argc , char* argv[]);
            bool FlagExists(const std::string& flag) const;
            std::string RetrieveValue(const std::string& flag);
            std::string RetrieveValue(CmndLineFlag flag);
            Argument RetrieveArgument(const std::string& flag);
            Argument RetrieveArgument(CmndLineFlag flag);
    };

}

#endif // !YE_COMMAND_LINE_ARGS_HPP