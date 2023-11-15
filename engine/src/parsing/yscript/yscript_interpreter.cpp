#include "parsing/yscript/yscript_interpreter.hpp"

#include "core/resource_handler.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "scene/systems.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/camera.hpp"

namespace YE {

namespace YS {
    
    glm::vec3 Interpreter::Vec3FromProperty(const Property& property) const {
        glm::vec3 vec;

        for (uint32_t i = 0; i < property.values.size(); ++i) {
            if (property.values[i].type != LiteralType::FLOAT)
                throw yscript_interpreter_error("Property value must be a float" /*, property.type.line , property.type.col */);
            vec[i] = property.values[i].value.floating_point;
        }
        return vec;
    }

    void Interpreter::ProcessScene(Node* node) {
        if (node->type != NodeType::SCENE)
            throw yscript_interpreter_error("UNDEFINED BEHAVIOR | PROCESSING NON SCENE AS SCENE" /*, node->id , node->type */);
        
        current_scene = ynew Scene(node->id); // Scene* new_scene = scene_graph.CreateScene(n->id);
        current_scene->InitializeScene();
        for (auto& c : node->children) 
            ProcessNode(current_scene , c);
    }

    void Interpreter::ProcessNode(Scene* scene , Node* node) {
        switch (node->type) {
            case NodeType::SCENE:
                throw yscript_interpreter_error("Scene node must be a root node" /*, node->id , node->type */);
            case NodeType::ENTITY: 
                current_entity = scene->CreateEntity(node->id);
            break;
            case NodeType::TRANSFORM: ConstructTransform(current_entity , node); break;
            case NodeType::RENDERABLE:
            case NodeType::TEXTURED_RENDERABLE:
            case NodeType::RENDERABLE_MODEL: ConstructRenderable(current_entity , node); break;
            case NodeType::POINT_LIGHT: ConstructLight(current_entity , node); break;
            case NodeType::CAMERA: ConstructCamera(node); break;
            case NodeType::SCRIPT: AttachScript(current_entity , node); break;
            case NodeType::PHYSICS_BODY: ConstructPhysics(current_entity , node); break;
            case NodeType::BOX_COLLIDER:
            case NodeType::SPHERE_COLLIDER:
            case NodeType::CAPSULE_COLLIDER: AttachCollider(current_entity , node); break;
            default: 
                throw yscript_interpreter_error("Invalid node type" /*, node->id , node->type */);
        }

        node_stack.push(node);
        for (auto& c : node->children) 
            ProcessNode(scene , c);
        node_stack.pop();
    }
    
    void Interpreter::ConstructTransform(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Transform node must be a child of an entity" /*, node->id , node->type */);
        
        auto& transform = current_entity->GetComponent<components::Transform>();
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::POSITION: transform.position = Vec3FromProperty(prop); break;
                case PropertyType::ROTATION: transform.rotation = Vec3FromProperty(prop); break;
                case PropertyType::SCALE: transform.scale = Vec3FromProperty(prop); break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }
    }

    void Interpreter::ConstructRenderable(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Renderable node must be a child of an entity" /*, node->id , node->type */);
        
        VertexArray* vao = nullptr;
        Material material;
        std::vector<Texture*> textures;
        std::string shader_name;
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::MESH: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("Mesh property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::STRING)
                        throw yscript_interpreter_error("Mesh property must be a string" /*, node->id , node->type */);
                    
                    vao = ResourceHandler::Instance()->GetPrimitiveVAO(*prop.values[0].value.string);
                    if (vao == nullptr)
                        throw yscript_interpreter_error("Invalid mesh name" /*, node->id , node->type */);
                } break;
                // case PropertyType::MATERIAL: break;
                case PropertyType::TEXTURE: {
                    if (prop.values[0].type != LiteralType::STRING)
                        throw yscript_interpreter_error("Texture property must be a string" /*, node->id , node->type */);

                    for (uint32_t i = 0; i < prop.values.size(); ++i) {
                        Texture* tex = ResourceHandler::Instance()->GetTexture(*prop.values[i].value.string);
                        if (tex == nullptr) {
                            ENGINE_WARN("Invalid texture name {}" , *prop.values[i].value.string);
                            continue;
                        }
                        textures.push_back(tex);
                    }
                } break;
                case PropertyType::SHADER: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("Shader property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::STRING)
                        throw yscript_interpreter_error("Shader property must be a string" /*, node->id , node->type */);
                    
                    shader_name = *prop.values[0].value.string;
                } break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }

        Material mat;
        switch (node->type) {
            case NodeType::RENDERABLE: {
                auto& renderable = current_entity->AddComponent<components::Renderable>(vao , mat , shader_name);
            } break;
            case NodeType::TEXTURED_RENDERABLE: {
                auto& renderable = current_entity->AddComponent<components::TexturedRenderable>(vao , mat , shader_name , textures);
            } break;
            case NodeType::RENDERABLE_MODEL: {
                // auto& renderable = current_entity->AddComponent<components::RenderableModel>(vao , shader_name , textures);
            } break;
            default:
                throw yscript_interpreter_error("Invalid node type" /*, node->id , node->type */);
        }
    }

    void Interpreter::ConstructLight(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Light node must be a child of an entity" /*, node->id , node->type */);
        
        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant = 0.0f;
        float linear = 0.0f;
        float quadratic = 0.0f;
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::COLOR: color = Vec3FromProperty(prop); break;
                case PropertyType::AMBIENT: ambient = Vec3FromProperty(prop); break;
                case PropertyType::DIFFUSE: diffuse = Vec3FromProperty(prop); break;
                case PropertyType::SPECULAR: specular = Vec3FromProperty(prop); break;
                case PropertyType::CONSTANT: constant = prop.values[0].value.floating_point; break;
                case PropertyType::LINEAR: linear = prop.values[0].value.floating_point; break;
                case PropertyType::QUADRATIC: quadratic = prop.values[0].value.floating_point; break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }
        auto& light = current_entity->AddComponent<components::PointLight>(color , ambient , diffuse , specular , constant , linear , quadratic);
    }
    
    void Interpreter::ConstructCamera(Node* node) {
        if (current_scene == nullptr)
            throw yscript_interpreter_error("Camera node must be a child of a scene" /*, node->id , node->type */);

        CameraType type = CameraType::PERSPECTIVE;
        Camera* cam = current_scene->AttachCamera(node->id , type);

        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::POSITION: cam->SetPosition(Vec3FromProperty(prop)); break;
                case PropertyType::FRONT: cam->SetFront(Vec3FromProperty(prop)); break;
                case PropertyType::UP: cam->SetUp(Vec3FromProperty(prop)); break;
                case PropertyType::RIGHT: cam->SetRight(Vec3FromProperty(prop)); break;
                case PropertyType::WORLD_UP: cam->SetWorldUp(Vec3FromProperty(prop)); break;
                case PropertyType::EULER_ANGLES: cam->SetOrientation(Vec3FromProperty(prop)); break;
                case PropertyType::VIEWPORT: break;
                case PropertyType::CLIP: break;
                case PropertyType::SPEED: cam->SetSpeed(prop.values[0].value.floating_point); break;
                case PropertyType::SENSITIVITY: cam->SetSensitivity(prop.values[0].value.floating_point); break;
                case PropertyType::FOV: cam->SetFOV(prop.values[0].value.floating_point); break;
                case PropertyType::ZOOM: cam->SetZoom(prop.values[0].value.floating_point);break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }
    }
    
    void Interpreter::AttachScript(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Script node must be a child of an entity" /*, node->id , node->type */);
        
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::OBJECT: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("Script object property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::STRING)
                        throw yscript_interpreter_error("Script object property must be a string" /*, node->id , node->type */);

                    auto& id = entity->GetComponent<components::ID>();

                    std::vector<ParamHandle> params{ &id.id };
                    auto& cs_script = entity->AddComponent<components::Script>(*prop.values[0].value.string , params);
                } break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }
    }

    void Interpreter::ConstructPhysics(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Physics node must be a child of an entity" /*, node->id , node->type */);

        PhysicsBodyType type;
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::PHYSICS_BODY_TYPE: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("PhysicsBodyType property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::STRING)
                        throw yscript_interpreter_error("PhysicsBodyType property must be a string" /*, node->id , node->type */);

                    std::string type_str = *prop.values[0].value.string; 
                    if (type_str == "static") {
                        type = PhysicsBodyType::STATIC;
                    } else if (type_str == "kinematic") {
                        type = PhysicsBodyType::KINEMATIC;
                    } else if (type_str == "dynamic") {
                        type = PhysicsBodyType::DYNAMIC;
                    } else {
                        throw yscript_interpreter_error("Invalid PhysicsBodyType" /*, node->id , node->type */);
                    }
                } break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }
        
        auto& body = entity->AddComponent<components::PhysicsBody>(type);
    }

    void Interpreter::AttachCollider(Entity* entity , Node* node) {
        if (node_stack.empty())
            throw yscript_interpreter_error("Collider node must be a child of an entity" /*, node->id , node->type */);

        glm::vec3 size;
        float radius = 1.f;
        float height = 1.f;
        for (auto& prop : node->properties) {
            switch (prop.type) {
                case PropertyType::SCALE: size = Vec3FromProperty(prop); break;
                case PropertyType::RADIUS: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("Radius property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::FLOAT)
                        throw yscript_interpreter_error("Radius property must be a float" /*, node->id , node->type */);

                    radius = prop.values[0].value.floating_point;
                } break;
                case PropertyType::HEIGHT: {
                    if (prop.values.size() > 1)
                        throw yscript_interpreter_error("Height property must have only one value" /*, node->id , node->type */);
                    if (prop.values[0].type != LiteralType::FLOAT)
                        throw yscript_interpreter_error("Height property must be a float" /*, node->id , node->type */);

                    height = prop.values[0].value.floating_point;
                } break;
                default:
                    throw yscript_interpreter_error("Invalid property type" /*, node->id , node->type */);
            }
        }

        switch (node->type) {
            case NodeType::BOX_COLLIDER: {
                auto& box_collider = entity->AddComponent<components::BoxCollider>();
            } break;
            case NodeType::SPHERE_COLLIDER: {
                auto& sphere_collider = entity->AddComponent<components::SphereCollider>(radius);
            } break;
            case NodeType::CAPSULE_COLLIDER: {
                auto& capsule_collider = entity->AddComponent<components::CapsuleCollider>(radius , height);
            } break;
            default:
                throw yscript_interpreter_error("Invalid node type" /*, node->id , node->type */);
        }
    }

    Scene* Interpreter::InterpretScript(bool dump_nodes) { 
        // YScriptTreePrinter printer;
        // for (const auto& node : ast)
        //     node->Walk(&printer);

        NodeBuilder builder;
        try {
            for (const auto& node : ast)
                node->Walk(&builder);
        } catch (const node_builder_exception& e) {
            ENGINE_ERROR("{}" , e.what());
            return nullptr;
        }
        
        project_metadata = builder.ProjectMetadata();
        
        try {
            nodes = builder.Nodes();

            for (auto& n : nodes) {
                if (n->type == NodeType::SCENE) {
                    ProcessScene(n);
                }
            }
        } catch (const yscript_interpreter_error& e) {
            ENGINE_ERROR("{}" , e.what());
            return nullptr;
        }
        
        return current_scene;
    }

} // namespace YS

} // namespace YE
