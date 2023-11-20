#ifndef ENGINEY_YSCRIPT_NODE_BUILDER_HPP
#define ENGINEY_YSCRIPT_NODE_BUILDER_HPP

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>

#include <spdlog/fmt/fmt.h>
#include <glm/glm.hpp>

#include "yscript_ast.hpp"
#include "yscript_parser.hpp"

// #include "scene/scene_graph.hpp"

namespace EngineY { 

    class node_builder_exception : public std::exception {
        std::string msg;

        public:
            node_builder_exception(const std::string& msg , uint32_t line , uint32_t col) 
                : msg(fmt::format(fmt::runtime("[Node Builder Error] :: {} [{} , {}]") , msg , line , col)) {}
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
        TITLE , MIN_SCALE , CLEAR_COLOR , FLAGS , COLOR_BITS , STENCIL_SIZE ,
            MULTISAMPLE_BUFFERS , MULTISAMPLE_SAMPLES , FULLSCREEN , VSYNC , RENDERING_TO_SCREEN , 
            ACCELERATED_VISUAL , // window properties
        SHADERS , TEXTURES , MODELS , // resourceproperties
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
        RESOURCES ,
        SCENES ,
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
            int integer;
            float floating_point;
            std::string* string;
        } value;
    };

    struct Property {
        PropertyType type;
        std::vector<Literal> values; // list length 1 means assign literal in list to all property values
    };

    struct RawProjectMetadata {
        std::string project_name = "";
        std::string window_title = "";
        std::vector<Property> properties = {};
        std::vector<Property> window_properties = {};
        std::vector<Property> resource_properties = {};
        std::vector<Literal> scene_list = {};
    };

    struct Node;
    
    using NodePtr = std::unique_ptr<Node>;

    struct Node {
        NodePtr* parent = nullptr;
        NodeType type = NodeType::INVALID;

        std::string id = "";

        // the node will own its children
        std::vector<NodePtr> children = {};
        std::vector<Property> properties = {};
    };


namespace util {

    static NodeType NodeTypeFromToken(const YScriptToken& token);
    static PropertyType PropertyTypeFromToken(const YScriptToken& token);

} // namespace util

} // namespace YS

    constexpr uint32_t kNumProjectProperties = 16;

namespace YS {

    // forward declare the interpreter cause that file includes this one
    class Interpreter;

} // namespace YS

    /// \todo refactor property parsing to reduce code repetition
    class NodeBuilder : public YScriptTreeWalker {
        YS::Interpreter* interpreter = nullptr;
        
        std::stack<YS::Literal> literal_stack;
        std::stack<YS::Property> property_stack;
        
        std::stack<YS::NodePtr*> node_stack;
        std::vector<YS::NodePtr> nodes = {};

        YS::RawProjectMetadata project_metadata;

        void PrintNode(const YS::NodePtr& node , uint32_t depth = 0) const;

        public:
            NodeBuilder(YS::Interpreter* interpreter)
                : interpreter(interpreter) {}
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
            virtual void WalkResource(ResourceStmnt& resource) override;
            virtual void WalkSceneList(SceneListStmnt& resource) override;
            virtual void WalkNodeDecl(NodeDeclStmnt& node_decl) override;
            virtual void WalkFunction(FunctionStmnt& function_decl) override;
            virtual void WalkVarDecl(VarDeclStmnt& var_decl) override;
            virtual void WalkNodeBody(NodeBodyStmnt& node_body) override;
            virtual void WalkBlock(BlockStmnt& scene) override;

            void DumpNodes() const;
            inline const YS::RawProjectMetadata ProjectMetadata() const { return project_metadata; }
    };

    

} // namespace EngineY

#endif // !YE_YSCRIPT_NODE_BUILDER_HPP
