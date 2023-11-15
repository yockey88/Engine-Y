#include "project/project_manager.hpp"

#include <filesystem>

#include <magic_enum/magic_enum.hpp>
#include <choc/text/choc_StringUtilities.h>

#include "core/command_line_args.hpp"
#include "core/defines.hpp"
#include "log.hpp"
#include "core/app.hpp"
#include "parsing/yscript/node_builder.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene.hpp"
#include "parsing/yscript/yscript_parser.hpp"
#include "parsing/yscript/yscript_interpreter.hpp"
#include "serialization/yscript_serializer.hpp"

namespace YE {

    ProjectManager* ProjectManager::singleton = nullptr;

    bool ProjectManager::FindCoreDirectories(
        EngineConfig& config , 
        CmndLineHandler& cmnd_line_handler
    ) {
        ENTER_FUNCTION_TRACE();

        std::string program_files;
#ifdef YE_PLATFORM_WIN
        try {
            program_files = std::getenv("ProgramFiles");
        } catch (const std::exception& e) {
            ENGINE_ERROR("Failed to retrieve ProgramFiles environment variable: {}", e.what());
            return false;
        }
#else
        YE_CRITICAL_ASSERTION(false , "UNIMPLEMENTED");
#endif
        if (!std::filesystem::exists(program_files + "/EngineY")) {
            ENGINE_ERROR("Failed to find EngineY directory");
            return false;
        }

        if (!cmnd_line_handler.FlagExists(CmndLineFlag::WORKING_DIR)) {
            config.working_directory = std::filesystem::current_path().string();
        } else {
            config.working_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::WORKING_DIR);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_PATH)) {
            if (std::filesystem::current_path().string() == config.project_name) {
                config.project_path = std::filesystem::current_path().string();
            } else if (std::filesystem::exists(config.working_directory + "/" + config.project_name)) {
                config.project_path = config.working_directory + "/" + config.project_name;
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / config.project_name
            )) {
                config.project_path = program_files + "/EngineY/" + config.project_name;
            } else {
                ENGINE_ERROR("Failed to find project directory");
                return false;
            }
        } else {
            config.project_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_PATH);
        }

        if (!cmnd_line_handler.FlagExists(CmndLineFlag::PROJECT_FILE)) {
            if (std::filesystem::exists(
                std::filesystem::path(config.working_directory) / 
                    config.project_name / (config.project_name + ".yproj")
            )) {
                config.project_file = config.working_directory + "/" + 
                    config.project_name + "/" + config.project_name + ".yproj";
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / 
                    config.project_name / (config.project_name + ".yproj")
            )) {
                config.project_file = program_files + "/EngineY/" + 
                    config.project_name + "/" + config.project_name + ".yproj";
            } else if (std::filesystem::exists(
                std::filesystem::path(config.project_path) / 
                    (config.project_name + ".yproj")
            )) {
                config.project_file = (std::filesystem::path(config.project_path) / 
                        (config.project_name + ".yproj")).string();
            } else {
                ENGINE_ERROR("Failed to find project file");
                return false;
            }
        } else {
            config.project_file = cmnd_line_handler.RetrieveValue(CmndLineFlag::PROJECT_FILE);
        }

        current_project_file = config.project_file;
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::MODULES_DIR)) {
            if (std::filesystem::exists(std::filesystem::path(config.project_path) / "modules")) {
                config.modules_directory = (std::filesystem::path(config.project_path) / "modules").string();
            } else if (std::filesystem::exists(
                std::filesystem::path(program_files) / "EngineY" / "bin" / "Debug" / "modules"
            )) {
                config.modules_directory = program_files + "/EngineY/bin/Debug/modules";
            } else {
                ENGINE_ERROR("Failed to find modules directory");
                return false;
            }
        } else {
            config.modules_directory = cmnd_line_handler.RetrieveValue(CmndLineFlag::MODULES_DIR);
        }
        
        if (!cmnd_line_handler.FlagExists(CmndLineFlag::MONO_CONFIG_PATH)) {
            config.mono_config_path = config.mono_dll_path;
        } else {
            config.mono_config_path = cmnd_line_handler.RetrieveValue(CmndLineFlag::MONO_CONFIG_PATH);
        } 

        EXIT_FUNCTION_TRACE();
    }

    bool ProjectManager::BuildProjectMetadata(const YS::RawProjectMetadata& raw_metadata) {
        for (auto& p : raw_metadata.properties) {
            switch (p.type) {
                case YS::PropertyType::AUTHOR:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Project author must be a single string");
                        return false; 
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project name must be a string");
                        return false;
                    } 
                    project_metadata.author = std::string{ *p.values[0].value.string };
                break;
                case YS::PropertyType::VERSION: {
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Project version must be in the format major.minor.patch");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project version must be a string");
                        return false;
                    }
                    std::string version = std::string{ *p.values[0].value.string };
                    if (version.find('.') == std::string::npos) {
                        ENGINE_ERROR("Project version must be in the format: major.minor.patch");
                        return false;
                    }
                    std::vector<std::string> version_parts = choc::text::splitString(version , '.' , false);
                    if (version_parts.empty()) {
                        ENGINE_ERROR("Project version must be in the format: major.minor.patch");
                        return false;
                    }
                    try {
                        project_metadata.version[0] = std::stoi(version_parts[0]);
                        if (version_parts.size() > 1)
                            project_metadata.version[1] = std::stoi(version_parts[1]);
                        if (version_parts.size() > 2)
                            project_metadata.version[2] = std::stoi(version_parts[2]);
                    } catch (const std::exception& e) {
                        ENGINE_ERROR("Failed to parse project version: {}", e.what());
                        return false;
                    }
                                                } break;
                case YS::PropertyType::DESCRIPTION:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Project description must be a single string");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project description must be a string");
                        return false;
                    }
                    project_metadata.description = std::string{ *p.values[0].value.string };
                break;
                case YS::PropertyType::RESOURCES:
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project resources must be a string");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project resources must be a string");
                        return false;
                    }
                    project_metadata.project_resources = std::string{ *p.values[0].value.string };
                break;
                case YS::PropertyType::PATH:
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project path must be a string");
                        return false;
                    }
                    project_metadata.project_path = std::string{ *p.values[0].value.string };
                break;
                default:
                    ENGINE_ERROR("Unrecognized property type: {}", magic_enum::enum_name(p.type));
                    return false;
            }
        }

        return true;
    }
    
    ProjectManager* ProjectManager::Instance() {
        if (singleton == nullptr)
            singleton = ynew ProjectManager;
        return singleton;
    }
    
    EngineConfig ProjectManager::LoadProject(
        App* application , CmndLineHandler& cmnd_line
    ) {

#ifdef YE_DEBUG_BUILD
        YE_CRITICAL_ASSERTION(
            application != nullptr , 
            "Attempting to register project with a null application"
        );
        cmnd_line.DumpArgs();
#endif

        EngineConfig app_config = application->GetEngineConfig();
        app_config.engine_root = std::filesystem::current_path().string(); 
        app_config.mono_dll_path = app_config.engine_root + "/external";

        if (cmnd_line.FlagExists(CmndLineFlag::PROJECT_NAME)) 
            app_config.project_name = cmnd_line.RetrieveValue(CmndLineFlag::PROJECT_NAME);
        else 
            app_config.project_name = application->ProjectName();

        if (app_config.project_name.empty()) {
            ENGINE_ERROR("Project name is empty");
            return app_config;
        }
        
        if (cmnd_line.FlagExists(CmndLineFlag::PROJECT_FILE)) { 
            app_config.use_project_file = true;
            app_config.project_file = cmnd_line.RetrieveValue(CmndLineFlag::PROJECT_FILE);
        }         
        
        if (!cmnd_line.FlagExists(CmndLineFlag::WORKING_DIR) || !cmnd_line.FlagExists(CmndLineFlag::PROJECT_PATH)   ||
            !cmnd_line.FlagExists(CmndLineFlag::MODULES_DIR)  || !cmnd_line.FlagExists(CmndLineFlag::MONO_CONFIG_PATH)) {
            if (!FindCoreDirectories(app_config , cmnd_line)) {
                ENGINE_ERROR("Failed to find core directories");
                return app_config;
            }
        } else {
            if (cmnd_line.FlagExists(CmndLineFlag::PROJECT_PATH)) 
                app_config.project_path = cmnd_line.RetrieveValue(CmndLineFlag::PROJECT_PATH);

            if (cmnd_line.FlagExists(CmndLineFlag::WORKING_DIR)) 
                app_config.working_directory = cmnd_line.RetrieveValue(CmndLineFlag::WORKING_DIR);

            if (cmnd_line.FlagExists(CmndLineFlag::MODULES_DIR))
                app_config.modules_directory = cmnd_line.RetrieveValue(CmndLineFlag::MODULES_DIR);
        
            if (cmnd_line.FlagExists(CmndLineFlag::PROJECT_BIN))
                app_config.project_bin = cmnd_line.RetrieveValue(CmndLineFlag::PROJECT_BIN);
        
            if (cmnd_line.FlagExists(CmndLineFlag::MONO_CONFIG_PATH))
                app_config.mono_config_path = cmnd_line.RetrieveValue(CmndLineFlag::MONO_CONFIG_PATH);
        
            if (cmnd_line.FlagExists(CmndLineFlag::ENGINE_ROOT))
                app_config.engine_root = cmnd_line.RetrieveValue(CmndLineFlag::ENGINE_ROOT);
        }

        app_config.config_is_valid = true;
        return app_config;
    }

    Scene* ProjectManager::LoadProjectFile(const std::string& ys_file) {
        ENTER_FUNCTION_TRACE();

        std::filesystem::path project_path = ys_file;
        if (!std::filesystem::exists(project_path)) {
            ENGINE_ERROR("Could not find project file: {0}" , project_path.string());
            return nullptr; 
        } else {
            ENGINE_INFO("Found project file: {0}" , project_path.string());
        }

        YScriptLexer lexer(project_path.string());

        auto [src , tokens] = lexer.Lex();

        YScriptParser parser(tokens);
        ProjectAst parse_tree = parser.Parse();

        YS::Interpreter interpreter(parse_tree);
        Scene* current_scene = interpreter.InterpretScript(true);

        if (current_scene == nullptr) {
            ENGINE_ERROR("Failed to interpret project script on load , aborting");
            return nullptr;
        }

        YS::RawProjectMetadata metadata = interpreter.ProjectMetadata();
        project_metadata.project_name = metadata.project_name;

        if (!BuildProjectMetadata(metadata)) {
            ENGINE_ERROR("Failed to build project metadata");
            return nullptr;
        }

        EXIT_FUNCTION_TRACE();        
        // temporary while I develop this system
        return current_scene; 
        // return true;
    }

    void ProjectManager::SaveCurrentProject() {
        /// \todo get the project scene graph

        // SceneGraph* application_scene_graph 
        Scene* project_scene = SceneManager::Instance()->CurrentScene();

        YScriptSerializer serializer;
        if (!serializer.SerializeScene(project_scene , current_project_file)) {
            ENGINE_ERROR("Failed to serialize scene");
            return;
        } else {
            ENGINE_INFO("Scene saved successfully");
        }
    }
            
    void ProjectManager::Cleanup() {
        if (singleton != nullptr) {
            ydelete singleton;
            singleton = nullptr;
        }
    }

} // namespace YE
