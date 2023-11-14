#include "core/command_line_args.hpp"

#include <magic_enum/magic_enum.hpp>

#include "log.hpp"
#include "core/hash.hpp"

namespace YE {

    bool CmndLineHandler::FlagValid(const std::string& flag) const {
        ENTER_FUNCTION_TRACE_MSG("{}" , flag);

        for (uint32_t i = 0; i < kNumValidFlags; ++i) {
            if (flag == kValidFlags[i])
                return true;
        }

        EXIT_FUNCTION_TRACE();
        return false;
    }
    
    bool CmndLineHandler::FlagRequiresValue(const std::string& flag) const {
        ENTER_FUNCTION_TRACE_MSG("{}" , flag);

        if      (flag == "--project-name"      || flag == "-n")   return true;
        else if (flag == "--project-file"      || flag == "-f")   return true;
        else if (flag == "--project-path"      || flag == "-p")   return true;
        else if (flag == "--working-directory" || flag == "-wd")  return true;
        else if (flag == "--modules-directory" || flag == "-md")  return true;
        else if (flag == "--project-bin"       || flag == "-pb")  return true;
        else if (flag == "--mono-config-path"  || flag == "-mcp") return true;
        else if (flag == "--engine-root"       || flag == "-er")  return true;
        
        EXIT_FUNCTION_TRACE();
        return false;
    }

    bool CmndLineHandler::ValidateArgs() const {
        ENTER_FUNCTION_TRACE();

        EXIT_FUNCTION_TRACE();
        return true;
    }

    std::string CmndLineHandler::ShortenArg(const std::string& arg) const {
        ENTER_FUNCTION_TRACE_MSG("{}" , arg);

        if (arg.length() <= 4) return arg;

        if      (arg == "--project-name")      return "-n";
        else if (arg == "--project-file")      return "-f";
        else if (arg == "--project-path")      return "-p";
        else if (arg == "--working-directory") return "-wd";
        else if (arg == "--modules-directory") return "-md";
        else if (arg == "--project-bin")       return "-pb";
        else if (arg == "--mono-config-path")  return "-mcp";
        else if (arg == "--engine-root")       return "-er";
        else if (arg == "--from-launcher")     return "-fl";

        YE_CRITICAL_ASSERTION(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
        return "";
    }

    void CmndLineHandler::DumpArgs() {
        ENTER_FUNCTION_TRACE();

        for (auto& [f , a] : args) {
            ENGINE_DEBUG("Flag: {} | Value: {}" , f , a.value);
        }

        EXIT_FUNCTION_TRACE();
    }
    
    bool CmndLineHandler::Parse(int argc , char* argv[]) {
        ENTER_FUNCTION_TRACE();

        std::vector<std::string> raw_args;

        ENGINE_DEBUG("Parsing {} command line arguments..." , argc);
        ENGINE_DEBUG("Executable path: {}", argv[0]);
        
        exe_path = std::string{ argv[0] };
        if (argc == 1) return true;

        for (int i = 1; i < argc; ++i) {
            raw_args.push_back(std::string{ argv[i] });
        }

        for (uint32_t i = 0; i < raw_args.size(); ++i) {
            std::string& arg = raw_args[i];

            ENGINE_DEBUG("Parsing argument: {}", arg);

            if (arg.length() < 2) {
                ENGINE_ERROR("Invalid command line argument: {}", arg);
                return false;
            }

            if (!FlagValid(arg)) {
                ENGINE_ERROR("Invalid command line argument: {}", arg);
                return false;
            } else if (FlagRequiresValue(arg)) {
                if (i + 1 >= raw_args.size()) {
                    ENGINE_ERROR("Invalid command line argument: {}", arg);
                    return false;
                }
                ++i;
                args[ShortenArg(arg)] = { arg , raw_args[i] };
                ENGINE_DEBUG("Flag: {} | Value: {}" , arg , raw_args[i]);
            } else {
                args[ShortenArg(arg)] = { arg , "" };
            }
        }

        EXIT_FUNCTION_TRACE();
        return ValidateArgs();
    }

    bool CmndLineHandler::FlagExists(const std::string& flag) const {
        for (auto& [f , a] : args) {
            if (flag == f)
                return true;
        }

        return false;
    }
    
    bool CmndLineHandler::FlagExists(CmndLineFlag flag) const {
        return FlagExists(std::string(kShortFlags[flag]));
    }

    std::string CmndLineHandler::RetrieveValue(const std::string& flag) {
        ENTER_FUNCTION_TRACE_MSG("{}" , flag);

        if (!FlagExists(flag) || !FlagValid(flag)) {
            ENGINE_ERROR("Attempting to retrieve value for nonexistent flag: {}", flag);
            return "";
        }

        if (FlagRequiresValue(flag)) {
            EXIT_FUNCTION_TRACE();

            return args[flag].value;
        } else {
            ENGINE_WARN("Attempting to retrieve value for flag that does not require value: {}", flag);
            EXIT_FUNCTION_TRACE();
            return "";
        }
    }

    std::string CmndLineHandler::RetrieveValue(CmndLineFlag flag) {
        if (flag >= CmndLineFlag::INVALID) {
            ENGINE_ERROR("Attempting to retrieve value for invalid flag: {}", flag);
            return "";
        }

        return RetrieveValue(std::string(kShortFlags[flag]));
    }
            
    Argument CmndLineHandler::RetrieveArgument(const std::string& flag) {
        ENTER_FUNCTION_TRACE_MSG("{}" , flag);

        if (!FlagExists(flag) || !FlagValid(flag)) {
            ENGINE_ERROR("Attempting to retrieve argument for nonexistent flag: {}", flag);
            return { "" , "" };
        }

        EXIT_FUNCTION_TRACE();
        return args[flag];
    }

    Argument CmndLineHandler::RetrieveArgument(CmndLineFlag flag) {
        ENTER_FUNCTION_TRACE_MSG("{}" , flag);

        if (flag >= CmndLineFlag::INVALID) {
            ENGINE_ERROR("Attempting to retrieve argument for invalid flag: {}", flag);
            return { "" , "" };
        }

        EXIT_FUNCTION_TRACE();
        return RetrieveArgument(std::string(kShortFlags[flag]));
    }

}