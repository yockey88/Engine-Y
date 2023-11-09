#include "core/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "log.hpp"

namespace YE {
    std::string Filesystem::build_config = BUILD_CONFIG;
    std::string Filesystem::script_engine_mono_path = MONO_DLLS_PATH;
    std::string Filesystem::script_engine_mono_config_path = MONO_CONFIG_PATH;
    
    std::string Filesystem::internal_modules_path = ""; 
    std::string Filesystem::project_modules_path = "";

    std::string Filesystem::engine_root = ENGINE_ROOT;
    std::string Filesystem::engine_code_dir = "";
    std::string Filesystem::engine_respath = "";
    std::string Filesystem::engine_modulepath = "";
    std::string Filesystem::engine_shaderpath = "";
    std::string Filesystem::engine_texturepath = "";
    std::string Filesystem::engine_modelpath = "";
    std::string Filesystem::gui_ini_path = "";
    
    std::string Filesystem::project_directory = ""; 
    std::string Filesystem::project_code_dir = "";
    std::string Filesystem::project_bin = "";
    std::string Filesystem::respath = "";
    std::string Filesystem::modulepath = "";
    
    std::string Filesystem::shaderpath = "";
    std::string Filesystem::texturepath = "";
    std::string Filesystem::modelpath = "";

    void Filesystem::Initialize(const std::string& project_name) {
        project_bin = "bin/"+ build_config + "/" + project_name;
        
        internal_modules_path = project_bin + "/modules.dll";
        project_modules_path = project_bin + "/" + project_name + "_modules.dll";

        engine_code_dir = engine_root + "/engine";
        engine_respath = engine_code_dir + "/resources";
        engine_modulepath = engine_respath + "/modules";
        engine_shaderpath = engine_respath + "/shaders";
        engine_texturepath = engine_respath + "/textures";
        engine_modelpath = engine_respath + "/models";
        gui_ini_path = engine_respath + "/imgui.ini";

        project_directory = GetCWD() + "/" + project_name;
        project_code_dir = project_directory + "/" + project_name;
        respath = project_code_dir + "/resources";    
        modulepath = project_code_dir + "/modules";
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

    void Filesystem::OverrideResourcePath(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            YE_ERROR("Attempting to override resource path with nonexistent path :: [{0}]" , path);
            return;
        }        
        respath = path;       
        shaderpath = respath + "/shaders";
        texturepath = respath + "/textures";
        modelpath = respath + "/models";
    }

}