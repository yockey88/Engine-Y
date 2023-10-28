#include "core/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "log.hpp"

namespace YE {
    std::string Filesystem::script_engine_mono_path = MONO_DLLS_PATH;
    std::string Filesystem::script_engine_mono_config_path = MONO_CONFIG_PATH;
    std::string Filesystem::internal_modules_path = PROJECT_BIN;
    std::string Filesystem::project_modules_path = PROJECT_BIN;
    std::string Filesystem::engine_core_path = "%APPDATA%\\EngineY";
    std::string Filesystem::engine_respath = ENGINE_RESOURCES_DIR;
    std::string Filesystem::engine_modulepath = "";
    std::string Filesystem::engine_shaderpath = "";
    std::string Filesystem::engine_texturepath = "";
    std::string Filesystem::engine_modelpath = "";
    std::string Filesystem::respath = PROJECT_RESOURCES_DIR;
    std::string Filesystem::modulepath = PROJECT_MODULES;
    std::string Filesystem::shaderpath = "";
    std::string Filesystem::texturepath = "";
    std::string Filesystem::modelpath = "";

    void Filesystem::Initialize() {

        internal_modules_path = internal_modules_path + "/modules.dll";
        project_modules_path = project_modules_path + "/" + PROJECT_NAME + "_modules.dll";
        engine_modulepath = engine_respath + "/modules";
        engine_shaderpath = engine_respath + "/shaders";
        engine_texturepath = engine_respath + "/textures";
        engine_modelpath = engine_respath + "/models";
        
        shaderpath = respath + "/shaders";
        texturepath = respath + "/textures";
        modelpath = respath + "/models";
    }

    bool Filesystem::FileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    void Filesystem::ReadFileAsCStr(char* buffer , const std::string& path) {
        std::string str = ReadFileAsStr(path);
        const char* cstr = str.c_str();
        memcpy(buffer , cstr , str.length() + 1);
        buffer[str.length() + 1] = '\0';
    }

    std::string Filesystem::ReadFileAsStr(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            YE_ERROR("Failed to open file :: [{0}]" , path);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string str = buffer.str();

        return str;
    }
    
    std::vector<char> Filesystem::ReadFileAsSBytes(const std::string& path) {
        std::ifstream file(path , std::ios::binary | std::ios::ate);

        YE_CRITICAL_ASSERTION(file.is_open() , "Failed to open file: {0}" , path);

        std::streampos end = file.tellg();
        file.seekg(0 , std::ios::beg);
        uint32_t size = static_cast<uint32_t>(end - file.tellg());

        if (size == 0) 
            return {};

        std::vector<char> buffer(size);
        file.read(buffer.data() , size);
        file.close();

        return buffer;
    }

}