#include "core/command_line_args.hpp"

#include "log.hpp"
#include "core/hash.hpp"

namespace YE {

    bool CmndLineHandler::FlagValid(const std::string& flag) const {
        for (uint32_t i = 0; i < kNumValidFlags; ++i) {
            if (flag == kValidFlags[i])
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

    std::string CmndLineHandler::ShortenArg(const std::string& arg) const {
        if (arg.length() <= 4) return arg;

        if      (arg == "--project-name")      return "-n";
        else if (arg == "--project-file")      return "-f";
        else if (arg == "--project-path")      return "-p";
        else if (arg == "--working-directory") return "-wd";
        else if (arg == "--modules-directory") return "-md";
        else if (arg == "--project-bin")       return "-pb";
        else if (arg == "--mono-dll-path")     return "-mdp";
        else if (arg == "--mono-config-path")  return "-mcp";
        else if (arg == "--engine-root")       return "-er";

        YE_CRITICAL_ASSERTION(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
        return "";
    }

    bool CmndLineHandler::Parse(int argc , char* argv[]) {
        std::vector<std::string> raw_args;

        exe_path = std::string{ argv[0] };
        if (argc == 1) return true;

        for (int i = 1; i < argc; ++i)
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
                args[arg] = { ShortenArg(arg) , raw_args[i] };
            } else {
                args[arg] = { ShortenArg(arg) , "" };
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

    std::string CmndLineHandler::RetrieveValue(CmndLineFlag flag) {
        if (flag >= CmndLineFlag::INVALID) {
            YE_ERROR("Attempting to retrieve value for invalid flag: {}", flag);
            return "";
        }

        return RetrieveValue(std::string(kShortFlags[flag]));
    }
            
    Argument CmndLineHandler::RetrieveArgument(const std::string& flag) {
        if (!FlagExists(flag) || !FlagValid(flag)) {
            YE_ERROR("Attempting to retrieve argument for nonexistent flag: {}", flag);
            return { "" , "" };
        }

        return args[flag];
    }

    Argument CmndLineHandler::RetrieveArgument(CmndLineFlag flag) {
        if (flag >= CmndLineFlag::INVALID) {
            YE_ERROR("Attempting to retrieve argument for invalid flag: {}", flag);
            return { "" , "" };
        }

        return RetrieveArgument(std::string(kShortFlags[flag]));
    }

}