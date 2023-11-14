#ifndef YE_YSCRIPT_INTERPRETER_HPP
#define YE_YSCRIPT_INTERPRETER_HPP

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "yscript_ast.hpp"
#include "yscript_parser.hpp"

#include "scene/scene_graph.hpp"

namespace YE { 

    class node_builder_exception : public std::exception {
        std::string msg;

        public:
            node_builder_exception(const std::string& msg , uint32_t line , uint32_t col) 
                : msg(fmt::format("[Node Builder Error] :: {} [{} , {}]" , msg , line , col)) {}
            ~node_builder_exception() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };

namespace YS {

    enum class LiteralType {
        BOOLEAN ,
        INTEGER ,
        FLOAT ,
        STRING
    };

    enum class PropertyType {
        AUTHOR , VERSION , DESCRIPTION , RESOURCES , PATH , // project properties
            MIN_SCALE , CLEAR_COLOR , FLAGS , COLOR_BITS , STENCIL_SIZE ,
            MULTISAMPLE_BUFFERS , MULTISAMPLE_SAMPLES , FULLSCREEN , VSYNC , RENDERING_TO_SCREEN , 
            ACCELERATED_VISUAL , 
        POSITION , SCALE , ROTATION , // transform properties
        MESH , TEXTURE , SHADER , MODEL , // renderable properties
        COLOR , AMBIENT , DIFFUSE , SPECULAR , CONSTANT , LINEAR , QUADRATIC , // light properties
        FRONT , UP , RIGHT , WORLD_UP , EULER_ANGLES , 
            VIEWPORT , CLIP , SPEED , SENSITIVITY , 
            FOV , ZOOM , // camera properties
        OBJECT , // script properties 
        PHYSICS_BODY_TYPE , // physics body properties
        RADIUS , HEIGHT , // collider properties
        
        INVALID
    };

    enum class NodeType {
        PROJECT , 
        WINDOW ,
        SCENE ,
        ENTITY ,
        TRANSFORM ,
        RENDERABLE , TEXTURED_RENDERABLE , RENDERABLE_MODEL ,
        POINT_LIGHT ,
        CAMERA ,
        SCRIPT , NATIVE_SCRIPT ,
        PHYSICS_BODY ,
        BOX_COLLIDER , SPHERE_COLLIDER , CAPSULE_COLLIDER , MODEL_COLLIDER ,

        INVALID
    };

    struct Literal {
        LiteralType type;
        union {
            bool boolean;
            float floating_point;
            std::string* string;
        } value;
    };

    struct Property {
        PropertyType type;
        std::vector<Literal> values; // list length 1 means assign literal in list to all property values
    };

    struct ProjectMetadata {
        std::string project_name = "";
        std::vector<Property> properties = {};
    };

    struct Node;
    class Function;

    struct Scope {};

    class Function {};

    struct Node {
        Node* parent = nullptr;
        NodeType type = NodeType::INVALID;

        std::string id = "";
        std::vector<Node*> children = {};
        std::vector<Property> properties = {};
        // std::vector<std::unique_ptr<Function>> functions;
    };

    class Environment {
        Environment(Environment&&) = delete;
        Environment(const Environment&) = delete;
        Environment& operator=(Environment&&) = delete;
        Environment& operator=(const Environment&) = delete;

        std::vector<Node*> nodes = {};

        public:
            Environment() {}
            ~Environment() {}
    };

namespace util {

    static NodeType NodeTypeFromToken(const YScriptToken& token);
    static PropertyType PropertyTypeFromToken(const YScriptToken& token);

} // namespace util

} // namespace YS

} // namespace YE

constexpr uint32_t kNumProjectProperties = 5;

namespace YE {

    class NodeBuilder : public YScriptTreeWalker {
        std::stack<YS::Literal> literal_stack;
        std::stack<YS::Property> property_stack;
        
        std::stack<YS::Node*> node_stack;
        std::vector<YS::Node*> nodes = {};

        YS::Node* current_node = nullptr;

        YS::ProjectMetadata project_metadata;

        void PrintNode(YS::Node* node , uint32_t depth = 0) const;
        void DeleteNode(YS::Node* node);

        public:
            virtual ~NodeBuilder();
            virtual void WalkLiteral(LiteralExpr& literal) override;
            virtual void WalkProperty(PropertyExpr& property) override;
            virtual void WalkAccess(AccessExpr& access) override;
            virtual void WalkLogical(LogicalExpr& logical) override;
            virtual void WalkUnary(UnaryExpr& unary) override;
            virtual void WalkBinary(BinaryExpr& binary) override;
            virtual void WalkCall(CallExpr& call) override;
            virtual void WalkGrouping(GroupingExpr& grouping) override;
            virtual void WalkVar(VarExpr& var) override;
            virtual void WalkAssign(AssignExpr& assign) override;
            virtual void WalkExpr(ExprStmnt& expr) override;
            virtual void WalkProject(ProjectMetadataStmnt& project) override;
            virtual void WalkWindow(WindowStmnt& window) override;
            virtual void WalkNodeDecl(NodeDeclStmnt& node_decl) override;
            virtual void WalkFunction(FunctionStmnt& function_decl) override;
            virtual void WalkVarDecl(VarDeclStmnt& var_decl) override;
            virtual void WalkNodeBody(NodeBodyStmnt& node_body) override;
            virtual void WalkBlock(BlockStmnt& scene) override;

            void DumpNodes() const;
            inline const YS::ProjectMetadata ProjectMetadata() const { return project_metadata; }
            inline std::vector<YS::Node*> Nodes() const { return nodes; }
    };

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
        std::vector<YS::Node*> nodes = {};
        
        std::stack<YS::Node*> node_stack;

        SceneGraph scene_graph;
        Scene* current_scene = nullptr;
        Entity* current_entity = nullptr;
        ProjectMetadata project_metadata;

        glm::vec3 Vec3FromProperty(const Property& property) const;

        void ProcessScene(YS::Node* node);
        void ProcessNode(Scene* scene , YS::Node* node);

        void ConstructTransform(Entity* entity , YS::Node* node);
        void ConstructRenderable(Entity* entity , YS::Node* node);
        void ConstructLight(Entity* entity , YS::Node* node);
        void ConstructCamera(YS::Node* node);
        void AttachScript(Entity* entity , YS::Node* node);
        void ConstructPhysics(Entity* entity , YS::Node* node);
        void AttachCollider(Entity* entity , YS::Node* node);

        public:
            Interpreter(std::vector<std::unique_ptr<ASTNode>>& ast) 
                : ast(std::move(ast)) {}
            ~Interpreter() {}

            Scene* BuildScene(bool dump_nodes = false);
            // SceneGraph BuildScene(bool dump_nodes = false);
            
            inline const YS::ProjectMetadata ProjectMetadata() const { return project_metadata; }
    };

} // namespace YS

} // namespace YE

#endif // !YE_YSCRIPT_INTERPRETER_HPP