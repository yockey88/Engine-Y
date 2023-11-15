#include "serialization/yscript_serializer.hpp"

#include "scene/entity.hpp"
#include "scene/scene.hpp"
#include "project/project_manager.hpp"

namespace YE {

    void YScriptSerializer::EmitYProjHeader(std::ofstream& file) {
        std::string project_name;
    }
    
    bool YScriptSerializer::SerializeProject(const ProjectMetadata* metadata) {
        return false;
    }

    bool YScriptSerializer::SerializeScene(Scene* scene , const std::string& filename) {
        if (scene == nullptr) 
            return false;
        
        std::ofstream file(filename);

        auto entities = scene->Entities();

        return false;
    }

} // namespace YE
