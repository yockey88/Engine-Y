#ifndef YE_COMMAND_LINE_ARGS_HPP
#define YE_COMMAND_LINE_ARGS_HPP

#include <string>
#include <map>

namespace YE {

    struct Argument {
        std::string flag;
        std::string value;
    };

    class CmndLineHandler {
        std::map<std::string , Argument> args;

        bool FlagValid(const std::string& flag) const;
        bool FlagRequiresValue(const std::string& flag) const;
        bool ValidateArgs() const;

        public:
            CmndLineHandler() {}
            ~CmndLineHandler() {}

            bool Parse(int argc , char* argv[]);
            bool FlagExists(const std::string& flag) const;
            std::string RetrieveValue(const std::string& flag);
    };

}

#endif // !YE_COMMAND_LINE_ARGS_HPP