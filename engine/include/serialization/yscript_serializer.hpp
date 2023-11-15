#ifndef YE_YSCRIPT_SERIALIZER_HPP
#define YE_YSCRIPT_SERIALIZER_HPP

#include <string>
#include <stack>
#include <fstream>

namespace YE {

    struct ProjectMetadata;
    class Scene;

    class YScriptSerializer {
        std::stack<char> tab_stack;

        void EmitYProjHeader(std::ofstream& file);

        public:
            YScriptSerializer() {}
            ~YScriptSerializer() {}
            
            bool SerializeProject(const ProjectMetadata* metadata /* SceneGraph* proj_scenegraph , ...*/);
            bool SerializeScene(Scene* scene , const std::string& filename);
            // bool SerializeSceneGraph(SceneGraph* scene_graph , const std::string& filename);
            
            // bool SerializeProject(const std::string
    };

} // namespace YE

#endif // !YE_YSCRIPT_SERIALIZER_HPP
