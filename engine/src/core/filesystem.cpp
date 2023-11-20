#include "core/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "core/log.hpp"

namespace EngineY {
    std::string Filesystem::build_config = "";
    std::string Filesystem::script_engine_mono_path = "external";
    std::string Filesystem::script_engine_mono_config_path = "external"; 
    
    std::string Filesystem::internal_modules_path = ""; 
    std::string Filesystem::project_modules_path = "";

    std::string Filesystem::engine_respath = "engine/resources";
    std::string Filesystem::engine_modulepath = "modules";
    std::string Filesystem::engine_shaderpath = "engine/resources/shaders";
    std::string Filesystem::engine_texturepath = "engine/resources/textures";
    std::string Filesystem::engine_modelpath = "engine/resources/models";
    std::string Filesystem::gui_ini_path = "engine/imgui.ini";
    
    std::string Filesystem::project_directory = ""; 
    std::string Filesystem::project_bin = "";
    std::string Filesystem::respath = "";
    std::string Filesystem::modulepath = "";
    
    std::string Filesystem::shaderpath = "";
    std::string Filesystem::texturepath = "";
    std::string Filesystem::modelpath = "";

    void Filesystem::Initialize(const ApplicationConfig& config) {
#ifdef ENGINEY_DEBUG
        build_config = "Debug";
#elif ENGINEY_RELEASE
        build_config = "Release";
#else
        ENGINE_ASSERT(false , "UNREACHABLE | NON-EXHAUSTIVE MATCH");
#endif
        
        internal_modules_path = "bin/" + build_config + "/EngineModules/EngineModules.dll";
        project_modules_path = config.modules_path;

        engine_respath = "engine/resources";
        engine_shaderpath = "engine/resources/shaders";
        engine_texturepath = "engine/resources/textures";
        engine_modelpath = "engine/resources/models";
        gui_ini_path = "engine/imgui.ini";

        project_directory = config.project_path;
        respath = project_directory + "/resources";    
        modulepath = project_directory + "/modules";
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

        ENGINE_ASSERT(file.is_open() , "Failed to open file: {0}" , path);

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
