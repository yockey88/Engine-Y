#include "parsing/yscript/node_builder.hpp"

#include <spdlog/fmt/fmt.h>

#include "parsing/yscript/yscript_interpreter.hpp"

namespace EngineY {

namespace YS {

namespace util {

    NodeType NodeTypeFromToken(const YScriptToken& token) {
        switch (token.type) {
            case YScriptTokenType::WINDOW: return NodeType::WINDOW;
            case YScriptTokenType::RESOURCES: return NodeType::RESOURCES;
            case YScriptTokenType::SCENES: return NodeType::SCENES;
            case YScriptTokenType::SCENE: return NodeType::SCENE;
            case YScriptTokenType::ENTITY: return NodeType::ENTITY;
            case YScriptTokenType::TRANSFORM: return NodeType::TRANSFORM;
            case YScriptTokenType::RENDERABLE: return NodeType::RENDERABLE;
            case YScriptTokenType::TEXTURED_RENDERABLE: return NodeType::TEXTURED_RENDERABLE;
            case YScriptTokenType::RENDERABLE_MODEL: return NodeType::RENDERABLE_MODEL;
            case YScriptTokenType::POINT_LIGHT: return NodeType::POINT_LIGHT;
            case YScriptTokenType::CAMERA: return NodeType::CAMERA;
            case YScriptTokenType::SCRIPT: return NodeType::SCRIPT;
            case YScriptTokenType::NATIVE_SCRIPT: return NodeType::NATIVE_SCRIPT;
            case YScriptTokenType::PHYSICS_BODY: return NodeType::PHYSICS_BODY;
            case YScriptTokenType::BOX_COLLIDER: return NodeType::BOX_COLLIDER;
            case YScriptTokenType::SPHERE_COLLIDER: return NodeType::SPHERE_COLLIDER;
            case YScriptTokenType::CAPSULE_COLLIDER: return NodeType::CAPSULE_COLLIDER;
            default:
                throw node_builder_exception("Invalid node type" , token.line , token.col);
        }
    }

    PropertyType PropertyTypeFromToken(const YScriptToken& token) {
        switch (token.type) {
            case YScriptTokenType::AUTHOR: return PropertyType::AUTHOR; 
            case YScriptTokenType::VERSION: return PropertyType::VERSION;
            case YScriptTokenType::DESCRIPTION: return PropertyType::DESCRIPTION;
            case YScriptTokenType::RESOURCES: return PropertyType::RESOURCES;
            case YScriptTokenType::PATH: return PropertyType::PATH;
            case YScriptTokenType::TITLE: return PropertyType::TITLE;
            case YScriptTokenType::MIN_SCALE: return PropertyType::MIN_SCALE;
            case YScriptTokenType::CLEAR_COLOR: return PropertyType::CLEAR_COLOR;  
            case YScriptTokenType::FLAGS: return PropertyType::FLAGS;
            case YScriptTokenType::COLOR_BITS: return PropertyType::COLOR_BITS;
            case YScriptTokenType::STENCIL_SIZE: return PropertyType::STENCIL_SIZE;
            case YScriptTokenType::MULTISAMPLE_BUFFERS: return PropertyType::MULTISAMPLE_BUFFERS;
            case YScriptTokenType::MULTISAMPLE_SAMPLES: return PropertyType::MULTISAMPLE_SAMPLES;
            case YScriptTokenType::FULLSCREEN: return PropertyType::FULLSCREEN; 
            case YScriptTokenType::VSYNC: return PropertyType::VSYNC;
            case YScriptTokenType::RENDERING_TO_SCREEN: return PropertyType::RENDERING_TO_SCREEN;
            case YScriptTokenType::ACCELERATED_VISUAL: return PropertyType::ACCELERATED_VISUAL; 
            case YScriptTokenType::SHADERS: return PropertyType::SHADERS;
            case YScriptTokenType::TEXTURES: return PropertyType::TEXTURES;
            case YScriptTokenType::MODELS: return PropertyType::MODELS;
            case YScriptTokenType::POSITION: return PropertyType::POSITION;
            case YScriptTokenType::ROTATION: return PropertyType::ROTATION;
            case YScriptTokenType::SCALE: return PropertyType::SCALE;
            case YScriptTokenType::MESH: return PropertyType::MESH;
            case YScriptTokenType::TEXTURE: return PropertyType::TEXTURE;
            case YScriptTokenType::SHADER: return PropertyType::SHADER;
            case YScriptTokenType::MODEL: return PropertyType::MODEL;
            case YScriptTokenType::COLOR: return PropertyType::COLOR;
            case YScriptTokenType::AMBIENT: return PropertyType::AMBIENT;
            case YScriptTokenType::DIFFUSE: return PropertyType::DIFFUSE;
            case YScriptTokenType::SPECULAR: return PropertyType::SPECULAR;
            case YScriptTokenType::CONSTANT: return PropertyType::CONSTANT;
            case YScriptTokenType::LINEAR: return PropertyType::LINEAR;
            case YScriptTokenType::QUADRATIC: return PropertyType::QUADRATIC;            
            case YScriptTokenType::FRONT: return PropertyType::FRONT;
            case YScriptTokenType::UP: return PropertyType::UP;
            case YScriptTokenType::RIGHT: return PropertyType::RIGHT;
            case YScriptTokenType::WORLD_UP: return PropertyType::WORLD_UP;
            case YScriptTokenType::EULER_ANGLES: return PropertyType::EULER_ANGLES;
            case YScriptTokenType::VIEWPORT: return PropertyType::VIEWPORT;
            case YScriptTokenType::CLIP: return PropertyType::CLIP;
            case YScriptTokenType::SPEED: return PropertyType::SPEED;
            case YScriptTokenType::SENSITIVITY: return PropertyType::SENSITIVITY;
            case YScriptTokenType::FOV: return PropertyType::FOV;
            case YScriptTokenType::ZOOM: return PropertyType::ZOOM;
            case YScriptTokenType::OBJECT: return PropertyType::OBJECT;
            case YScriptTokenType::PHYSICS_BODY_TYPE: return PropertyType::PHYSICS_BODY_TYPE;
            case YScriptTokenType::RADIUS: return PropertyType::RADIUS;
            case YScriptTokenType::HEIGHT: return PropertyType::HEIGHT;
            default:
                throw node_builder_exception("Invalid property type" , token.line , token.col);
        }
    }

} // namespace util

} // namespace YS

    /// ************ Node Builder ************ /// 
    void NodeBuilder::PrintNode(const YS::NodePtr& node , uint32_t depth) const {
        std::string indent = "";
        for (uint32_t i = 0; i < depth; ++i) indent += "  ";

        std::string node_type = "";
        switch (node->type) {
            case YS::NodeType::SCENE: node_type = "Scene"; break;
            case YS::NodeType::ENTITY: node_type = "Entity"; break;
            case YS::NodeType::TRANSFORM: node_type = "Transform"; break;
            case YS::NodeType::RENDERABLE: node_type = "Renderable"; break;
            case YS::NodeType::TEXTURED_RENDERABLE: node_type = "TexturedRenderable"; break;
            case YS::NodeType::RENDERABLE_MODEL: node_type = "RenderableModel"; break;
            case YS::NodeType::POINT_LIGHT: node_type = "PointLight"; break;
            case YS::NodeType::CAMERA: node_type = "Camera"; break;
            case YS::NodeType::SCRIPT: node_type = "Script"; break;
            case YS::NodeType::PHYSICS_BODY: node_type = "Physics Body"; break;
            default: node_type = "Invalid"; break;
        }

        std::string node_id = node->id;

        std::string node_properties; // = fmt::format("Properties (size {}) : " , node->properties.size());
        if (node->properties.size() > 0) {
            node_properties += "\n" + indent + fmt::format(fmt::runtime("  |>> Properties (size {}) : ") , node->properties.size());
            for (uint32_t j = 0; j < node->properties.size(); ++j) {
                YS::Property prop = node->properties[j];
                node_properties += fmt::format(fmt::runtime("{} ({}) : [ ") , magic_enum::enum_name(prop.type) , prop.values.size());
                for (uint32_t i = 0; i < prop.values.size(); ++i) {
                     EngineY::YS::Literal val = prop.values[i];
                    switch (val.type) {
                        case YS::LiteralType::BOOLEAN:
                            node_properties += fmt::format(fmt::runtime("{}") , val.value.boolean);
                        break;
                        case YS::LiteralType::FLOAT:
                            node_properties += fmt::format(fmt::runtime("{}") , val.value.floating_point);
                        break;
                        case YS::LiteralType::STRING: node_properties += *val.value.string; break;
                        default: node_properties += fmt::format(fmt::runtime("Invalid\n")); break;
                    }
                    if (i != prop.values.size() - 1) node_properties += " , ";
                }
                node_properties += " ]";
                if (j != node->properties.size() - 1) node_properties += " | ";
            }
        }

        std::cout << fmt::format(fmt::runtime("{}{} [{}] : {}\n") , indent , node_type , node_id , node_properties);

        for (uint32_t i = 0; i < node->children.size(); ++i) {
            PrintNode(node->children[i] , depth + 1);
        }
    }

    NodeBuilder::~NodeBuilder() {
        interpreter->GiveNodes(nodes);
    }

    void NodeBuilder::WalkLiteral(LiteralExpr& literal) {
        YS::Literal lit;
        switch (literal.value.type) {
            case YScriptTokenType::TRUE_TKN: [[fallthrough]];
            case YScriptTokenType::FALSE_TKN: 
                lit.type = YS::LiteralType::BOOLEAN;
                lit.value.boolean = (literal.value.value == "true");
            break;
            case YScriptTokenType::INTEGER:
                lit.type = YS::LiteralType::INTEGER;
                try {
                    lit.value.integer = std::stoi(literal.value.value);
                } catch (const std::invalid_argument&) {
                    throw node_builder_exception("Invalid integer literal" , literal.value.line , literal.value.col);
                }
            break;
            case YScriptTokenType::FLOAT:
                lit.type = YS::LiteralType::FLOAT;
                try {
                    lit.value.floating_point = std::stof(literal.value.value);
                } catch (const std::invalid_argument&) {
                    throw node_builder_exception("Invalid float literal" , literal.value.line , literal.value.col);
                }
            break;
            case YScriptTokenType::STRING:
                lit.type = YS::LiteralType::STRING;
                lit.value.string = &literal.value.value;
            break;
            default:
                throw node_builder_exception("Invalid literal type" , literal.value.line , literal.value.col);
        }

        literal_stack.push(lit);
    }

    void NodeBuilder::WalkProperty(PropertyExpr& property) {
        YS::Property prop;
        prop.type = YS::util::PropertyTypeFromToken(property.type);
        for (auto& v : property.values) {
            v->Walk(this);
            prop.values.push_back(literal_stack.top());
            literal_stack.pop();
        }

        for (uint32_t i = 0; i < prop.values.size(); ++i) {
            for (uint32_t j = i; j < prop.values.size(); ++j) {
                switch (prop.values[i].type) {
                    case YS::LiteralType::BOOLEAN:
                        if (prop.values[j].type != YS::LiteralType::BOOLEAN)
                            throw node_builder_exception("Property value list contains incompatible types" , property.type.line , property.type.col);
                    break;
                    case YS::LiteralType::INTEGER: [[fallthrough]];
                    case YS::LiteralType::FLOAT:
                        if (prop.values[j].type != YS::LiteralType::INTEGER && prop.values[j].type != YS::LiteralType::FLOAT)
                            throw node_builder_exception("Property value list contains incompatible types" , property.type.line , property.type.col);
                    break;
                    case YS::LiteralType::STRING:
                        if (prop.values[j].type != YS::LiteralType::STRING)
                            throw node_builder_exception("Property value list contains incompatible types" , property.type.line , property.type.col);
                    break;
                    default:
                        throw node_builder_exception("Invalid literal type" , property.type.line , property.type.col);
                }
            }
        }

        property_stack.push(prop);
    }

    void NodeBuilder::WalkAccess(AccessExpr& access) {
    }

    void NodeBuilder::WalkLogical(LogicalExpr& logical) {
    }

    void NodeBuilder::WalkUnary(UnaryExpr& unary) {
        unary.right->Walk(this);
        if (literal_stack.empty())
            throw node_builder_exception("Unary expression must have a literal" , unary.op.line , unary.op.col);

        YS::Literal lit = literal_stack.top();
        literal_stack.pop();

        switch (unary.op.type) {
            case YScriptTokenType::MINUS:
                if (lit.type == YS::LiteralType::FLOAT)
                    lit.value.floating_point *= -1.0f;
                else
                    throw node_builder_exception("Unary expression must have a numeric literal" , unary.op.line , unary.op.col);
            break;
            case YScriptTokenType::EXCLAMATION:
                if (lit.type == YS::LiteralType::BOOLEAN)
                    lit.value.boolean = !lit.value.boolean;
                else
                    throw node_builder_exception("Unary expression must have a boolean literal" , unary.op.line , unary.op.col);
            break;
            default:
                throw node_builder_exception("Invalid unary operator" , unary.op.line , unary.op.col);
        }

        literal_stack.push(lit);
    }

    void NodeBuilder::WalkBinary(BinaryExpr& binary) {
    }

    void NodeBuilder::WalkCall(CallExpr& call) {
    }

    void NodeBuilder::WalkGrouping(GroupingExpr& grouping) {
    }

    void NodeBuilder::WalkVar(VarExpr& var) {
    }

    void NodeBuilder::WalkAssign(AssignExpr& assign) {
    }
    
    void NodeBuilder::WalkExpr(ExprStmnt& expr) {
    }
    
    void NodeBuilder::WalkProject(ProjectMetadataStmnt& project) {
        std::vector<YS::Property> proj_properties;

        for (auto& property : project.metadata) {
            property->Walk(this);
            proj_properties.push_back(property_stack.top());
            property_stack.pop();
        }

        project_metadata.project_name = project.identifier.value;
        project_metadata.properties = proj_properties;
    }

    void NodeBuilder::WalkWindow(WindowStmnt& window) {
        for (auto& property : window.description) {
            property->Walk(this);
            project_metadata.window_properties.push_back(property_stack.top());
            property_stack.pop();
        }

        project_metadata.window_title = window.identifier.value;
    }

    void NodeBuilder::WalkResource(ResourceStmnt& resource) {
        for (auto& property : resource.description) {
            property->Walk(this);
            project_metadata.resource_properties.push_back(property_stack.top());
            property_stack.pop();
        }
    }
    
    void NodeBuilder::WalkSceneList(SceneListStmnt& scene_list) {
        for (auto& path : scene_list.description) {
            path->Walk(this);
            project_metadata.scene_list.push_back(literal_stack.top());
            literal_stack.pop();
        }
    }

    void NodeBuilder::WalkFunction(FunctionStmnt& function_decl) {
    }

    void NodeBuilder::WalkVarDecl(VarDeclStmnt& var_decl) {
    }

    void NodeBuilder::WalkNodeDecl(NodeDeclStmnt& node_decl) {
        YS::NodePtr node = std::make_unique<YS::Node>();
        node->type = YS::util::NodeTypeFromToken(node_decl.type);
        node->id = node_decl.identifier.value;
        
        // walk the current node
        //  we push a pointer to the unique ptr bc there is no reason to move the unique ptr 
        //  (we might actually be able to push a dummy object just to signify that we are in a node)
        {  
            node_stack.push(&node);

            // dummy object might be the way to go this is a little gross right here
            YS::NodePtr*& current_node = node_stack.top();

            if (node_decl.initializer != nullptr) {
                node_decl.initializer->Walk(this);
            }

            node_stack.pop();
        }

        while (property_stack.size() > 0) {
            node->properties.push_back(property_stack.top());
            property_stack.pop();
        }
        
        {
            if (node_stack.empty()) {
                // these are the final nodes so we can move the pointer here 
                nodes.push_back(std::move(node));
            } else {
                // first move the node into the children so it has a permanent home
                YS::NodePtr* current_node = std::move(node_stack.top());
                node->parent = current_node;
                (*current_node)->children.push_back(std::move(node));
                
            }
        }
    }

    void NodeBuilder::WalkNodeBody(NodeBodyStmnt& node_assign) {
        for (auto& stmnt : node_assign.description) {
            stmnt->Walk(this);
        }
    }
    
    void NodeBuilder::WalkBlock(BlockStmnt& scene) {
    }

    void NodeBuilder::DumpNodes() const {
        for (auto& node : nodes) {
            PrintNode(node , 0);
        }
    }

}
