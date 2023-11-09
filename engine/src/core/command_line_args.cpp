#include "core/command_line_args.hpp"

#include "log.hpp"
#include "core/hash.hpp"

static const std::vector<std::string> valid_flags = {
    "-pf" , "--project-file" , "-p" , "--project-path"
};

static constexpr uint32_t kNumValidFlags = 4;

namespace YE {

    bool CmndLineHandler::FlagValid(const std::string& flag) const {
        for (uint32_t i = 0; i < kNumValidFlags; ++i) {
            if (flag == valid_flags[i])
                return true;
        }
        return false;
    }
    
    bool CmndLineHandler::FlagRequiresValue(const std::string& flag) const {
        return false;
    }

    bool CmndLineHandler::ValidateArgs() const {
        return true;
    }

    bool CmndLineHandler::Parse(int argc , char* argv[]) {
        std::vector<std::string> raw_args;
        for (int i = 0; i < argc; ++i)
            raw_args.push_back(argv[i]);

        for (uint32_t i = 0; i < raw_args.size(); ++i) {
            std::string& arg = raw_args[i];
            if (arg.length() < 2) {
                YE_ERROR("Invalid command line argument: {}", arg);
                return false;
            }

            if (!FlagValid(arg)) {
                YE_ERROR("Invalid command line argument: {}", arg);
                return false;
            } else if (FlagRequiresValue(arg)) {
               if (i + 1 >= raw_args.size()) {
                    YE_ERROR("Invalid command line argument: {}", arg);
                    return false;
                }
                ++i;
                args[arg] = { arg , raw_args[i] };
            } else {
                args[arg] = { arg , "" };
            }
        }

        return ValidateArgs();
    }

    bool CmndLineHandler::FlagExists(const std::string& flag) const {
        for (auto& [f , a] : args) {
            if (flag == f)
                return true;
        }
        return false;
    }
    
    std::string CmndLineHandler::RetrieveValue(const std::string& flag) {
        if (!FlagExists(flag) || !FlagValid(flag)) {
            YE_ERROR("Attempting to retrieve value for nonexistent flag: {}", flag);
            return "";
        }

        if (FlagRequiresValue(flag)) {
            return args[flag].value;
        } else {
            YE_WARN("Attempting to retrieve value for flag that does not require value: {}", flag);
            return "";
        }
    }

}