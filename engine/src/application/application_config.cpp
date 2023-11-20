#include "application/application_config.hpp"

#include <choc/text/choc_StringUtilities.h>

#include "core/defines.hpp"
#include "core/log.hpp"
#include "core/command_line_args.hpp"
#include "parsing/yscript/node_builder.hpp"

namespace EngineY {

    ApplicationConfig ConfigBuilder::BuildLauncherConfig() {
        config.project_name = "launcher";
        config.project_path = "./launcher";
        config.project_file = "launcher/launcher.yproj";
        config.modules_path = "bin/Debug/launcher/launcher_modules.dll";

        config.window_config.title = "Engine Y Launcher";
        config.window_config.clear_color = { 0.1f , 0.1f , 0.1f , 1.0f };
        config.window_config.flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        return config;
    }
    
    bool ConfigBuilder::BuildProjectMetadata(const YS::RawProjectMetadata& raw_metadata) {
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

        WindowConfig window_config = {};
        window_config.title = raw_metadata.window_title;

        for (auto& p : raw_metadata.window_properties) {
            switch (p.type) {
                case YS::PropertyType::TITLE:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window title must be a single string");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Window title must be a string");
                        return false;
                    }
                    window_config.title = *p.values[0].value.string;
                break;
                case YS::PropertyType::SCALE:
                    if (p.values.size() != 2) {
                        ENGINE_ERROR("Window scale must be a list of two integers");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::INTEGER ||
                        p.values[1].type != YS::LiteralType::INTEGER) {
                        ENGINE_ERROR("Window scale must be a list of two integers");
                        return false;
                    }
                    window_config.size = glm::ivec2{ p.values[0].value.integer , p.values[1].value.integer };
                break;
                case YS::PropertyType::MIN_SCALE:
                    if (p.values.size() != 2) {
                        ENGINE_ERROR("Minimum window scale must be a list of two integers");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::INTEGER ||
                        p.values[1].type != YS::LiteralType::INTEGER) {
                        ENGINE_ERROR("Mimimum window scale must be a list of two integers");
                        return false;
                    }
                    window_config.min_size = glm::ivec2{ p.values[0].value.integer , p.values[1].value.integer };
                break;
                case YS::PropertyType::CLEAR_COLOR:
                    if (p.values.size() != 4) {
                        ENGINE_ERROR("Window clear color must be a list of four floating point values");
                        return false;
                    }
                    if (
                        p.values[0].type != YS::LiteralType::FLOAT ||
                        p.values[1].type != YS::LiteralType::FLOAT ||
                        p.values[2].type != YS::LiteralType::FLOAT ||
                        p.values[3].type != YS::LiteralType::FLOAT
                    ) {
                        ENGINE_ERROR("Window clear color must be a list of four floating point values");
                        return false;
                    }
                    window_config.clear_color = glm::vec4{ 
                        p.values[0].value.floating_point , p.values[1].value.floating_point ,
                        p.values[2].value.floating_point , p.values[3].value.floating_point 
                    };
                break;
                case YS::PropertyType::FLAGS:
                    for (auto& v : p.values) {
                        if (v.type != YS::LiteralType::INTEGER) {
                            ENGINE_ERROR("Window flags must be a list of integers");
                            return false;
                        }
                        window_config.flags |= v.value.integer;
                    }
                break;
                case YS::PropertyType::COLOR_BITS:
                    if (p.values.size() != 4) {
                        ENGINE_ERROR("Window color bits must be a list of four integers");
                        return false;
                    }
                    if (
                        p.values[0].type != YS::LiteralType::INTEGER ||
                        p.values[1].type != YS::LiteralType::INTEGER ||
                        p.values[2].type != YS::LiteralType::INTEGER ||
                        p.values[3].type != YS::LiteralType::INTEGER
                    ) {
                        ENGINE_ERROR("Window color bits must be a list of four integers");
                        return false;
                    }
                    window_config.gl_color_bits = glm::ivec4{ 
                        p.values[0].value.integer , p.values[1].value.integer ,
                        p.values[2].value.integer , p.values[3].value.integer 
                    };
                break;
                case YS::PropertyType::STENCIL_SIZE:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window stencil size must be a single integer");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::INTEGER) {
                        ENGINE_ERROR("Window stencil size must be an integer");
                        return false;
                    }
                    window_config.gl_stencil_size = p.values[0].value.integer;
                break;
                case YS::PropertyType::MULTISAMPLE_BUFFERS:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window multisample buffers must be a single integer");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::INTEGER) {
                        ENGINE_ERROR("Window multisample buffers must be an integer");
                        return false;
                    }
                    if (p.values[0].value.integer != 0 && p.values[0].value.integer != 1) {
                        ENGINE_ERROR("Window multisample buffers must be either 0 or 1");
                        return false;
                    }
                    window_config.gl_multisample_buffers = p.values[0].value.integer;
                break;
                case YS::PropertyType::MULTISAMPLE_SAMPLES:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window multisample samples must be a single integer");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::INTEGER) {
                        ENGINE_ERROR("Window multisample samples must be an integer");
                        return false;
                    }
                    if (p.values[0].value.integer < 0 || p.values[0].value.integer > 16) {
                        ENGINE_ERROR("Window multisample samples must be between 0 and 16");
                        return false;
                    }
                    window_config.gl_multisample_samples = p.values[0].value.integer;
                break;
                case YS::PropertyType::FULLSCREEN:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window fullscreen must be a single boolean");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::BOOLEAN) {
                        ENGINE_ERROR("Window fullscreen must be a boolean");
                        return false;
                    }
                    window_config.fullscreen = p.values[0].value.boolean;
                break;
                case YS::PropertyType::VSYNC:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window vsync must be a single boolean");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::BOOLEAN) {
                        ENGINE_ERROR("Window vsync must be a boolean");
                        return false;
                    }
                    window_config.vsync = p.values[0].value.boolean;
                break;
                case YS::PropertyType::RENDERING_TO_SCREEN:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window rendering to screen must be a single boolean");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::BOOLEAN) {
                        ENGINE_ERROR("Window rendering to screen must be a boolean");
                        return false;
                    }
                    window_config.rendering_to_screen = p.values[0].value.boolean;
                break;
                case YS::PropertyType::ACCELERATED_VISUAL:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Window accelerated visual must be a single boolean");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::BOOLEAN) {
                        ENGINE_ERROR("Window accelerated visual must be a boolean");
                        return false;
                    }
                    window_config.accelerated_visual = p.values[0].value.boolean;
                break;
                default:
                    ENGINE_ERROR("Unrecognized window property type: {}", magic_enum::enum_name(p.type));
                    return false;
            }    
        }

        config.window_config = window_config;

        for (auto& p : raw_metadata.resource_properties) {
            switch (p.type) {
                case YS::PropertyType::SHADERS:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Project shaders must be a single string");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project shaders must be a string");
                        return false;
                    }
                    project_metadata.project_shader_dir = std::string{ *p.values[0].value.string };
                break;
                case YS::PropertyType::TEXTURES:
                    if (p.values.size() != 1) {
                        ENGINE_ERROR("Project textures must be a single string");
                        return false;
                    }
                    if (p.values[0].type != YS::LiteralType::STRING) {
                        ENGINE_ERROR("Project textures must be a string");
                        return false;
                    }
                    project_metadata.project_texture_dir = std::string{ *p.values[0].value.string };
                break;
                case YS::PropertyType::MODELS:
                    ENGINE_ASSERT(false , "UNIMPLEMENTED");
                break;
                default:
                    ENGINE_ERROR("Unrecognized resource property type: {}", magic_enum::enum_name(p.type));
                    return false;
            }
        }

        project_metadata.project_shader_dir = project_metadata.project_path + "/" + project_metadata.project_shader_dir;
        project_metadata.project_texture_dir = project_metadata.project_path + "/" + project_metadata.project_texture_dir;
        project_metadata.project_model_dir = project_metadata.project_path + "/" + project_metadata.project_model_dir;

        for (auto& p : raw_metadata.scene_list) {
            if (p.type != YS::LiteralType::STRING) {
                ENGINE_ERROR("Scene list must be a list of strings");
                return false;
            } 
            project_metadata.scenes.push_back(std::string{ *p.value.string });
        }

        config.scenes = project_metadata.scenes;

        return true;
    }

    ConfigBuilder::ConfigBuilder(CmndLineHandler* args) {
        if (args->FlagExists(CmndLineFlag::PROJECT_NAME)) 
            config.project_name = args->RetrieveValue(CmndLineFlag::PROJECT_NAME);

        if (config.project_name.empty()) {
            ENGINE_ERROR("Project name is empty");
            return;
        }
        
        if (args->FlagExists(CmndLineFlag::PROJECT_FILE)) { 
            config.use_project_file = true;
            config.project_file = args->RetrieveValue(CmndLineFlag::PROJECT_FILE);
        }         
        
        if (args->FlagExists(CmndLineFlag::PROJECT_PATH)) 
            config.project_path = args->RetrieveValue(CmndLineFlag::PROJECT_PATH);

        if (args->FlagExists(CmndLineFlag::MODULES_PATH))
            config.modules_path = args->RetrieveValue(CmndLineFlag::MODULES_PATH); 

        if (args->FlagExists(CmndLineFlag::IN_EDITOR)) {
            config.window_config.rendering_to_screen = false;
        }


        ProjectAst project_ast = {};

        YScriptLexer lexer(config.project_file);
        auto [src , tokens] = lexer.Lex();

        YScriptParser parser(tokens);
        ProjectAst parse_tree = parser.Parse();

        YS::Interpreter interpreter(parse_tree);
        interpreter.InterpretScript();

        YS::RawProjectMetadata raw_metadata = interpreter.ProjectMetadata(); 
        project_metadata.project_name = raw_metadata.project_name;

        if (!BuildProjectMetadata(raw_metadata)) {
            ENGINE_ERROR("Failed to build project metadata");
            return;
        }

        config.config_is_valid = true;
    }

    ApplicationConfig ConfigBuilder::Build() {
        if (!config.config_is_valid) {
            return BuildLauncherConfig();
        } else {
            return config;
        }

        ENGINE_ASSERT(false , "UNREACHABLE");
    }

} // namespace EngineY
