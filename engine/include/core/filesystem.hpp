#ifndef YE_FILESYSTEM_HPP
#define YE_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "core/defines.hpp"

namespace YE {

    struct EngineConfig;
    class Filesystem {
        static std::string build_config;
        static std::string script_engine_mono_path;
        static std::string script_engine_mono_config_path;
        static std::string internal_modules_path;
        static std::string project_modules_path;

        static std::string engine_root;
        static std::string engine_code_dir;
        static std::string engine_respath;
        static std::string engine_modulepath;
        static std::string engine_shaderpath;
        static std::string engine_texturepath;
        static std::string engine_modelpath;
        static std::string gui_ini_path;

        static std::string project_directory;
        static std::string project_code_dir;
        static std::string project_bin;
        static std::string respath;
        static std::string modulepath;
        static std::string shaderpath;
        static std::string texturepath;
        static std::string modelpath;

        friend struct EngineConfig;

        public:
            static void Initialize(const EngineConfig& config);
            static bool FileExists(const std::string& path);
            static void ReadFileAsCStr(char* buffer , const std::string& path);
            static std::string ReadFileAsStr(const std::string& path);
            static std::vector<char> ReadFileAsSBytes(const std::string& path);

            static void OverrideResourcePath(const std::string& path);
            static void OverrideProjectModulePath(const std::string& path);

            inline static std::filesystem::path GetCWDPath() { return std::filesystem::current_path(); }
            inline static std::string GetCWD() { return std::filesystem::current_path().string(); }

            inline static std::string GetMonoPath() { return script_engine_mono_path; }
            inline static std::string GetMonoConfigPath() { return script_engine_mono_config_path; }
            inline static std::string GetInternalModulesPath() { return internal_modules_path; }
            inline static std::string GetProjectModulesPath() { return project_modules_path; }

            inline static std::string GetEngineRoot() { return engine_root; }
            inline static std::string GetEngineDir() { return engine_code_dir; }
            inline static std::string GetEngineResPath() { return engine_respath; }
            inline static std::string GetEngineModulePath() { return engine_modulepath; }
            inline static std::string GetEngineShaderPath() { return engine_shaderpath; }
            inline static std::string GetEngineTexturePath() { return engine_texturepath; }
            inline static std::string GetEngineModelPath() { return engine_modelpath; }
            inline static std::string GetGuiIniPath() { return gui_ini_path; }
            inline static const char* GetGuiIniPathCStr() { return gui_ini_path.c_str(); }

            inline static std::string GetResPath() { return respath; }
            inline static std::string GetModulePath() { return modulepath; }
            inline static std::string GetShaderPath() { return shaderpath; }
            inline static std::string GetTexturePath() { return texturepath; }
            inline static std::string GetModelPath() { return modelpath; }
    };

}

#endif