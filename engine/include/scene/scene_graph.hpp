#ifndef ENGINEY_SCENE_GRAPH_HPP
#define ENGINEY_SCENE_GRAPH_HPP

#include <vector>

#include "scene/scene.hpp"

namespace EngineY {

    struct SceneNode {
        Scene* scene = nullptr;
        std::vector<SceneNode*> connections = {};
    };

    class SceneGraph {

        std::vector<SceneNode*> nodes = {};

        public:
            SceneGraph() {}
            SceneGraph(const std::string& ys_file);
            ~SceneGraph();

            void PushScene(Scene* scene);
            void PushScene(Scene* scene , const std::string& name);

            Scene* GetScene(const std::string& name);

            void Connect(const std::string& parent , const std::string& child);
    };

} // namespace YE

#endif // !YE_SCENE_GRAPH_HPP