#ifndef YE_PROJECT_MANAGER_HPP
#define YE_PROJECT_MANAGER_HPP

#include <string>

#include "core/defines.hpp"
#include "core/command_line_args.hpp"
#include "parsing/yscript/node_builder.hpp"

namespace YE {

    class App;
    class CmndLineHandler;
    class Scene;

    struct ProjectMetadata {
        std::string project_name;
        std::string author;
        std::string description;
        std::string project_path;
        std::string project_resources;
        uint32_t version[3] = { 0 , 0 , 0 };
    };

    class ProjectManager {
         static ProjectManager* singleton;

        App* project_context = nullptr;
        ProjectMetadata project_metadata;
        std::string current_project_file;
        
        ProjectManager() {}
        ~ProjectManager() {}
        
        ProjectManager(ProjectManager&) = delete;
        ProjectManager(const ProjectManager&) = delete;
        ProjectManager& operator=(ProjectManager&) = delete;
        ProjectManager& operator=(const ProjectManager&) = delete;

        bool FindCoreDirectories(
            EngineConfig& config , 
            CmndLineHandler& cmnd_line_handler
        );
        
        bool BuildProjectMetadata(const YS::RawProjectMetadata& raw_metadata);
        
        public:

            static ProjectManager* Instance();

            EngineConfig LoadProject(
                App* application ,
                CmndLineHandler& cmnd_line
            );
            Scene* LoadProjectFile(const std::string& ys_file);

            void SaveCurrentProject();

            void Cleanup();

            inline const ProjectMetadata& CurrentProjectMetadata() const { return project_metadata; }
            inline const std::string CurrentProjectFile() const { return current_project_file; }
    };

} // namespace YE

#endif // !YE_PROJECT_MANAGER_HPP
