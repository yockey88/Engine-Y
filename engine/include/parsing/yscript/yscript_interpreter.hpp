#ifndef YE_YSCRIPT_INTERPRETER_HPP
#define YE_YSCRIPT_INTERPRETER_HPP

#include <stdexcept>

#include <spdlog/fmt/fmt.h>

#include "yscript_ast.hpp"
#include "node_builder.hpp"

namespace YE {

    class Entity;
    class Scene;

namespace YS {

    class yscript_interpreter_error : public std::exception {
        std::string msg;

        public:
            yscript_interpreter_error(const std::string& msg) 
                : msg(fmt::format("[YScript Interpreter Error] :: {}" , msg)) {}
            ~yscript_interpreter_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };
    
    struct VM {};

    class Interpreter {
        std::vector<std::unique_ptr<ASTNode>> ast = {};
        std::vector<Node*> nodes = {};
        
        std::stack<Node*> node_stack;

        // SceneGraph scene_graph;
        Scene* current_scene = nullptr;
        Entity* current_entity = nullptr;
        RawProjectMetadata project_metadata;

        glm::vec3 Vec3FromProperty(const Property& property) const;

        void ProcessScene(Node* node);
        void ProcessNode(Scene* scene , Node* node);

        void ConstructTransform(Entity* entity , Node* node);
        void ConstructRenderable(Entity* entity , Node* node);
        void ConstructLight(Entity* entity , Node* node);
        void ConstructCamera(Node* node);
        void AttachScript(Entity* entity , Node* node);
        void ConstructPhysics(Entity* entity , Node* node);
        void AttachCollider(Entity* entity , Node* node);

        public:
            Interpreter(std::vector<std::unique_ptr<ASTNode>>& ast) 
                : ast(std::move(ast)) {}
            ~Interpreter() {}

            Scene* InterpretScript(bool dump_nodes = false);
            // SceneGraph BuildScene(bool dump_nodes = false);

            inline const RawProjectMetadata ProjectMetadata() const { return project_metadata; }
    };

} // namespace YS

} // namespace YE

#endif // !YE_YSCRIPT_INTERPRETER_HPP
