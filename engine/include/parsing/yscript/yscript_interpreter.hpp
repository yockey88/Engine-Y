#ifndef ENGINEY_YSCRIPT_INTERPRETER_HPP
#define ENGINEY_YSCRIPT_INTERPRETER_HPP

#include <stdexcept>

#include <spdlog/fmt/fmt.h>

#include "yscript_ast.hpp"
#include "node_builder.hpp"

namespace EngineY {

    class Entity;
    class Scene;

namespace YS {

    class yscript_interpreter_error : public std::exception {
        std::string msg;

        public:
            yscript_interpreter_error(const std::string& msg) 
                : msg(fmt::format(fmt::runtime("[YScript Interpreter Error] :: {}") , msg)) {}
            ~yscript_interpreter_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };
    
    struct VM {};

    class Interpreter {
        std::vector<std::unique_ptr<ASTNode>> ast = {};
        std::vector<YS::NodePtr> nodes = {};
        
        std::stack<YS::Node*> node_stack;

        // SceneGraph scene_graph;
        Scene* current_scene = nullptr;
        Entity* current_entity = nullptr;
        RawProjectMetadata project_metadata;

        bool nodes_built = false;

        glm::vec3 Vec3FromProperty(const Property& property) const;

        void ProcessScene(NodePtr& node);
        void ProcessNode(Scene* scene , NodePtr& node);

        void ConstructTransform(Entity* entity , NodePtr& node);
        void ConstructRenderable(Entity* entity , NodePtr& node);
        void ConstructLight(Entity* entity , NodePtr& node);
        void ConstructCamera(NodePtr& node);
        void AttachScript(Entity* entity , NodePtr& node);
        void ConstructPhysics(Entity* entity , NodePtr& node);
        void AttachCollider(Entity* entity , NodePtr& node);

        public:
            Interpreter(std::vector<std::unique_ptr<ASTNode>>& ast)
                : ast(std::move(ast)) {}
            ~Interpreter() {}

            void InterpretScript(bool dump_nodes = false);
            // SceneGraph BuildScene(bool dump_nodes = false);

            void GiveNodes(std::vector<YS::NodePtr>& nodes);

            inline const RawProjectMetadata ProjectMetadata() const { return project_metadata; }
            inline const bool NodesBuilt() const { return nodes_built; }
    };

} // namespace YS

} // namespace YE

#endif // !YE_YSCRIPT_INTERPRETER_HPP
