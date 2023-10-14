#include "scene/scene_graph.hpp"

#include "parsing/yscript/node_builder.hpp"

namespace YE {

    SceneGraph::SceneGraph(const std::string& ys_file) {
        std::string project_path = "sandbox/sandbox.ysc";
        YE::YScriptLexer lexer(project_path);
        YE::YScriptTokenPrinter printer;

        auto [src , tokens] = lexer.Lex();

        YE::YScriptParser parser(tokens);
        std::vector<std::unique_ptr<YE::ASTNode>> ast = parser.Parse();

        YE::YS::Interpreter interpreter(ast);
    }

    SceneGraph::~SceneGraph() {
        for (SceneNode* node : nodes) {
            if (node != nullptr) {
                if (node->scene != nullptr) {
                    node->scene->UnloadScene();
                    node->scene->Shutdown();
                    ydelete node->scene;
                }
                ydelete node;
            }
        }
    }

    void SceneGraph::PushScene(Scene* scene) {
        if (scene == nullptr)
            return;

        SceneNode* node = ynew SceneNode;
        node->name = scene->SceneName();
        node->scene = scene;
    }

    void SceneGraph::PushScene(Scene* scene , const std::string& name) {
        if (scene == nullptr)
            return;

        SceneNode* node = ynew SceneNode;
        node->name = name;
        node->scene = scene;
    }

    Scene* SceneGraph::GetScene(const std::string& name) {
        for (SceneNode* node : nodes) {
            if (node->name == name)
                return node->scene;
        }

        return nullptr;
    }

    void SceneGraph::Connect(const std::string& parent , const std::string& child) {
        SceneNode* parent_node = nullptr;
        SceneNode* child_node = nullptr;

        for (SceneNode* node : nodes) {
            if (node->name == parent)
                parent_node = node;
            if (node->name == child)
                child_node = node;
        }

        if (parent_node != nullptr && child_node != nullptr) {
            parent_node->connections.push_back(child_node);
        }
    }

} // namespace YE