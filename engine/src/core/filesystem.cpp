#include "core/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "log.hpp"

namespace YE {
    std::string Filesystem::build_config = "";
    std::string Filesystem::script_engine_mono_path = "";
    std::string Filesystem::script_engine_mono_config_path = "";
    
    std::string Filesystem::internal_modules_path = ""; 
    std::string Filesystem::project_modules_path = "";

    std::string Filesystem::engine_root = "";
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

    void Filesystem::Initialize(const EngineConfig& config) {
#ifdef YE_DEBUG_BUILD
        build_config = "Debug";
#elif YE_RELEASE_BUILD
        build_config = "Release";
#else
        YE_CRITICAL_ASSERTION(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
#endif
        script_engine_mono_path = config.mono_dll_path;
        script_engine_mono_config_path = config.mono_config_path;

        engine_root = config.engine_root; 

        project_bin =  config.project_path + "/bin/"+ build_config + "/" + config.project_name;
        
        internal_modules_path = engine_root + "/bin/" + build_config + "/modules" + "/modules.dll";
        project_modules_path = project_bin + "/" + config.project_name + "_modules.dll";

        engine_code_dir = engine_root + "/engine";
        engine_respath = engine_code_dir + "/resources";
        engine_modulepath = engine_respath + "/modules";
        engine_shaderpath = engine_respath + "/shaders";
        engine_texturepath = engine_respath + "/textures";
        engine_modelpath = engine_respath + "/models";
        gui_ini_path = engine_respath + "/imgui.ini";

        project_directory = config.project_path;
        project_code_dir = project_directory + "/" + config.project_name;
        respath = project_code_dir + "/resources";    
        modulepath = project_code_dir + "/modules";
        shaderpath = respath + "/shaders";
        texturepath = respath + "/textures";
        modelpath = respath + "/models";

    }

    /// \todo make this check engine directories and project dirs it knows about
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
            ENGINE_ERROR("Failed to open file :: [{0}]" , path);
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
            ENGINE_ERROR("Attempting to override resource path with nonexistent path :: [{0}]" , path);
            return;
        }        
        respath = path;       
        shaderpath = respath + "/shaders";
        texturepath = respath + "/textures";
        modelpath = respath + "/models";
    }

    void Filesystem::OverrideProjectModulePath(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            ENGINE_ERROR("Attempting to override module path with nonexistent path :: [{0}]" , path);
            return;
        }

        project_modules_path = path;
    }

}