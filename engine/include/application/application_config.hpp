#ifndef ENGINEY_APPLICATION_CONFIG_HPP
#define ENGINEY_APPLICATION_CONFIG_HPP

#include <string>

#include <glm/glm.hpp>
#include <SDL.h>

#include "parsing/yscript/yscript_interpreter.hpp"

namespace EngineY {

    class CmndLineHandler;

    struct WindowConfig {
        glm::ivec2 position = glm::ivec2(SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED);
        glm::ivec2 min_size = glm::ivec2(200 , 200);
        glm::ivec2 size = glm::ivec2(800 , 600);
        glm::vec4 clear_color = glm::vec4(0.2f , 0.3f , 0.3f , 1.0f);
        uint32_t flags = 0;
        glm::ivec4 gl_color_bits = glm::ivec4(8 , 8 , 8 , 8);
        uint32_t gl_stencil_size = 8;
        uint32_t gl_multisample_buffers = 1;
        uint32_t gl_multisample_samples = 16;
        std::string title = "Engine Y";
        
        bool fullscreen = false;
        bool vsync = false;
        bool rendering_to_screen = true;
        bool accelerated_visual = true;
    };

    struct ApplicationConfig {
        bool config_is_valid = false;

        WindowConfig window_config;

        std::string project_name = "";
        std::string project_path = "";
        std::string project_file = "";
        std::string modules_path = "";
        
        std::vector<std::string> scenes;
        
        bool use_project_file = false;
    };    

    struct ProjectMetadata {
        std::string project_name;
        std::string author;
        std::string description;
        std::string project_path;
        std::string project_shader_dir;
        std::string project_texture_dir;
        std::string project_model_dir;
        std::vector<std::string> scenes;
        uint32_t version[3] = { 0 , 0 , 0 };
    };

    class ConfigBuilder {
        CmndLineHandler* args;
        ApplicationConfig config;
        ProjectMetadata project_metadata;

        bool valid = false;

        ApplicationConfig BuildLauncherConfig();
        bool BuildProjectMetadata(const YS::RawProjectMetadata& raw_metadata);

        public:
            ConfigBuilder(CmndLineHandler* args);
            ApplicationConfig Build();

            inline const ProjectMetadata ProjectMetadata() const { return project_metadata; }
            inline bool IsValid() const { return valid; }
    };

} // namespace EngineY


#endif // !ENGINEY_APPLICATION_CONFIG_HPP
