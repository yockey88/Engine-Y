#include "core/command_line_args.hpp"

#include <magic_enum/magic_enum.hpp>

#include "core/log.hpp"
#include "core/hash.hpp"

namespace EngineY {

    bool CmndLineHandler::FlagValid(const std::string& flag) const {
        for (uint32_t i = 0; i < kNumValidFlags; ++i) {
            if (flag == kValidFlags[i])
                return true;
        }

        return false;
    }
    
    bool CmndLineHandler::FlagRequiresValue(const std::string& flag) const {
        if      (flag == "--project-name" || flag == "-n")   return true;
        else if (flag == "--project-file" || flag == "-f")   return true;
        else if (flag == "--project-path" || flag == "-p")   return true;
        else if (flag == "--modules-path" || flag == "-mp")  return true;
        
        return false;
    }

    bool CmndLineHandler::ValidateArgs() const {
        return true;
    }

    std::string CmndLineHandler::ShortenArg(const std::string& arg) const {
        if (arg.length() <= 4) return arg;

        if      (arg == "--project-name") return "-n";
        else if (arg == "--project-file") return "-f";
        else if (arg == "--project-path") return "-p";
        else if (arg == "--modules-path") return "-mp";
        else if (arg == "--in-editor")    return "-e";

        return "";
    }

    const void CmndLineHandler::DumpArgs() const {
        for (const auto& [f , a] : args) {
            ENGINE_DEBUG("Flag: {} | Value: {}" , f , a.value);
        }
    }
    
    CmndLineHandler::CmndLineHandler(int argc , char* argv[]) {
        std::vector<std::string> raw_args;

        ENGINE_DEBUG("Parsing {} command line arguments..." , argc);
        ENGINE_DEBUG("Executable path: {}", argv[0]);
        
        exe_path = std::string{ argv[0] };
        if (argc == 1) {
            args_valid = true;
            return;
        }

        for (int i = 1; i < argc; ++i) {
            raw_args.push_back(std::string{ argv[i] });
        }

        for (uint32_t i = 0; i < raw_args.size(); ++i) {
            std::string& arg = raw_args[i];

            ENGINE_DEBUG("Parsing argument: {}", arg);

            if (arg.length() < 2) {
                ENGINE_ERROR("Invalid command line argument: {}", arg);
                return;
            }

            if (!FlagValid(arg)) {
                ENGINE_ERROR("Invalid command line argument: {}", arg);
                return;
            } else if (FlagRequiresValue(arg)) {
                if (i + 1 >= raw_args.size()) {
                    ENGINE_ERROR("Invalid command line argument: {}", arg);
                    return;
                }
                ++i;
                args[ShortenArg(arg)] = { arg , raw_args[i] };
                ENGINE_DEBUG("Flag: {} | Value: {}" , arg , raw_args[i]);
            } else {
                args[ShortenArg(arg)] = { arg , "" };
            }
        }

        args_valid = ValidateArgs();
    }

    bool CmndLineHandler::ArgsValid() const {
        return args_valid;
    }

    const bool CmndLineHandler::FlagExists(const std::string& flag) const {
        for (auto& [f , a] : args) {
            if (flag == f)
                return true;
        }

        return false;
    }
    
    const bool CmndLineHandler::FlagExists(CmndLineFlag flag) const {
        return FlagExists(std::string(kShortFlags[flag]));
    }

    const std::string CmndLineHandler::RetrieveValue(const std::string& flag) {
        if (!FlagExists(flag) || !FlagValid(flag)) {
            ENGINE_ERROR("Attempting to retrieve value for nonexistent flag: {}", flag);
            return "";
        }

        if (FlagRequiresValue(flag)) {
            return args[flag].value;
        } else {
            ENGINE_WARN("Attempting to retrieve value for flag that does not require value: {}", flag);
            return "";
        }
    }

    const std::string CmndLineHandler::RetrieveValue(CmndLineFlag flag) {
        if (flag >= CmndLineFlag::INVALID) {
            return "";
        }

        return RetrieveValue(std::string(kShortFlags[flag]));
    }
            
    const Argument CmndLineHandler::RetrieveArgument(const std::string& flag) {
        if (!FlagExists(flag) || !FlagValid(flag)) {
            ENGINE_ERROR("Attempting to retrieve argument for nonexistent flag: {}", flag);
            return { "" , "" };
        }

        return args[flag];
    }

    const Argument CmndLineHandler::RetrieveArgument(CmndLineFlag flag) {
        if (flag >= CmndLineFlag::INVALID) {
            ENGINE_ERROR("Attempting to retrieve argument for invalid flag: {}", flag);
            return { "" , "" };
        }

        return RetrieveArgument(std::string(kShortFlags[flag]));
    }

} // namespace EngineY
