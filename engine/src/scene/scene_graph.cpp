#include "scene/scene_graph.hpp"

#include "core/defines.hpp"
#include "parsing/yscript/node_builder.hpp"

namespace EngineY {

    SceneGraph::SceneGraph(const std::string& ys_file) {
    }

    SceneGraph::~SceneGraph() {
        for (SceneNode* node : nodes) {
            if (node != nullptr) {
                if (node->scene != nullptr) {
                    node->scene->UnloadScene();
                    node->scene->Shutdown();
                    ydelete(node->scene);
                }
                ydelete(node);
            }
        }
    }

    void SceneGraph::PushScene(Scene* scene) {
    }

    void SceneGraph::PushScene(Scene* scene , const std::string& name) {
    }

    Scene* SceneGraph::GetScene(const std::string& name) {
        return nullptr;
    }

    void SceneGraph::Connect(const std::string& parent , const std::string& child) {
        // SceneNode* parent_node = nullptr;
        // SceneNode* child_node = nullptr;

        // for (SceneNode* node : nodes) {
        //     if (node->name == parent)
        //         parent_node = node;
        //     if (node->name == child)
        //         child_node = node;
        // }

        // if (parent_node != nullptr && child_node != nullptr) {
        //     parent_node->connections.push_back(child_node);
        // }
    }

} // namespace YE
