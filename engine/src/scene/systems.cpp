#include "scene/systems.hpp"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "log.hpp"
#include "engine.hpp"
#include "core/UUID.hpp"
#include "core/hash.hpp"
#include "core/resource_handler.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "rendering/shader.hpp"
#include "physics/physics_engine.hpp"

namespace YE {
    
    EntityCreatedSignal Systems::entity_created_signal{};
    EntityDestroyedSignal Systems::entity_destroyed_signal{};
    SceneLoadSignal Systems::scene_load_signal{};
    SceneUnloadSignal Systems::scene_unload_signal{};
    SceneStartSignal Systems::scene_start_signal{};
    SceneStopSignal Systems::scene_stop_signal{};
    UpdateSignal Systems::update_signal{};
    EntityUpdateSignal Systems::entity_update_transform_signal{};
    PhysicsBodyUpdateSignal Systems::physics_body_update_signal{};
    RenderableUpdateSignal Systems::renderable_update_signal{};
    TexturedRenderableUpdateSignal Systems::textured_renderable_update_signal{};
    RenderableModelUpdateSignal Systems::renderable_model_update_signal{};

    EntityCreatedSink Systems::create_entity_sink{ Systems::entity_created_signal };
    EntityDestroyedSink Systems::destroy_entity_sink{ Systems::entity_destroyed_signal };
    SceneLoadSink Systems::scene_load_sink{ Systems::scene_load_signal };
    SceneUnloadSink Systems::scene_unload_sink{ Systems::scene_unload_signal };
    SceneStartSink Systems::scene_start_sink{ Systems::scene_start_signal };
    SceneStopSink Systems::scene_stop_sink{ Systems::scene_stop_signal };
    UpdateSink Systems::update_sink{ Systems::update_signal };
    EntityUpdateSink Systems::update_entity_sink{ Systems::entity_update_transform_signal };
    PhysicsBodyUpdateSink Systems::physics_body_update_sink{ Systems::physics_body_update_signal };
    RenderableUpdateSink Systems::renderable_update_sink{ Systems::renderable_update_signal };
    TexturedRenderableUpdateSink Systems::textured_renderable_update_sink{ Systems::textured_renderable_update_signal };
    RenderableModelUpdateSink Systems::renderable_model_update_sink{ Systems::renderable_model_update_signal };

    void Systems::LoadShader(Shader *& shader , const std::string& entity_name , const std::string& shader_name , bool& corrupted) {
        ENTER_FUNCTION_TRACE();

        shader = ResourceHandler::Instance()->GetShader(shader_name);
        if (shader == nullptr)
            shader = ResourceHandler::Instance()->GetCoreShader(shader_name);
        
        if (shader == nullptr) {
            ENGINE_WARN("Entity [{0}] has corrupt renderable, could not find shader [{1}]" , entity_name , shader_name);
            corrupted = true;
        }

        EXIT_FUNCTION_TRACE();
    }

    void Systems::Initialize() {
        ENTER_FUNCTION_TRACE();

        create_entity_sink.connect<&EntityCreated>();
        destroy_entity_sink.connect<&EntityDestroyed>();
        scene_load_sink.connect<&LoadShaders>();
        scene_load_sink.connect<&BindScripts>();
        scene_unload_sink.connect<&UnbindScripts>();
        scene_unload_sink.connect<&SceneUnload>();
        scene_start_sink.connect<&ScenePlay>();
        scene_stop_sink.connect<&StopScene>();
        update_sink.connect<&UpdateScene>();
        update_entity_sink.connect<&UpdateTransform>();
        physics_body_update_sink.connect<&UpdatePhysicsBody>();
        renderable_update_sink.connect<&UpdateRenderable>();
        textured_renderable_update_sink.connect<&UpdateTexturedRenderable>();
        renderable_model_update_sink.connect<&UpdateRenderableModel>();

        EXIT_FUNCTION_TRACE();
    }

    void Systems::SetSceneContext(Scene* context) {
        ENTER_FUNCTION_TRACE();

        auto& registry = context->registry;
        registry.on_construct<entt::entity>().connect<&EntityConstructed>();
        registry.on_construct<components::RenderableModel>().connect<&ModelCreated>();
        registry.on_construct<components::PhysicsBody>().connect<&PhysicsBodyCreated>();
        registry.on_construct<components::BoxCollider>().connect<&BoxColliderCreated>();
        registry.on_construct<components::SphereCollider>().connect<&SphereColliderCreated>();
        registry.on_construct<components::CapsuleCollider>().connect<&CapsuleColliderCreated>();
        registry.on_construct<components::MeshCollider>().connect<&MeshColliderCreated>();
        
        registry.on_destroy<components::PhysicsBody>().connect<&PhysicsBodyDestroyed>();
        registry.on_destroy<components::BoxCollider>().connect<&BoxColliderDestroyed>();
        registry.on_destroy<components::SphereCollider>().connect<&SphereColliderDestroyed>();
        registry.on_destroy<components::CapsuleCollider>().connect<&CapsuleColliderDestroyed>();
        registry.on_destroy<components::MeshCollider>().connect<&MeshColliderDestroyed>();

        EXIT_FUNCTION_TRACE();
    }

    void Systems::EntityConstructed(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        registry.emplace<components::ID>(entity);
        registry.emplace<components::Transform>(entity);
        registry.emplace<components::Grouping>(entity);

        EXIT_FUNCTION_TRACE();
    }

    void Systems::EntityCreated(Scene* context , const std::string& name) {
        ENTER_FUNCTION_TRACE();

        Entity* entity = ynew Entity(context);
        auto& id = entity->GetComponent<components::ID>();
        id.name = name;
        id.id = Hash::FNV(name);

        context->entities[id.id] = entity;

        EXIT_FUNCTION_TRACE();
    }

    void Systems::ModelCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();
    
        auto& model = registry.get<components::RenderableModel>(entity);
        auto& transform = registry.get<components::Transform>(entity);

        if (model.model_name == "" || model.shader_name == "") {
            EXIT_FUNCTION_TRACE();
            return;
        }
        
        model.model = ResourceHandler::Instance()->GetModel(model.model_name);
        if (model.model == nullptr) {
            ENGINE_WARN("Entity has corrupt renderable, could not find model [{0}]" , model.model_name);
            model.corrupted = true;
            EXIT_FUNCTION_TRACE();
            return;
        }

        model.shader = ResourceHandler::Instance()->GetShader(model.shader_name);
        if (model.shader == nullptr)
            model.shader = ResourceHandler::Instance()->GetCoreShader(model.shader_name);

        if (model.shader == nullptr) {
            ENGINE_WARN("Entity with model [{0}] has corrupt renderable, could not find shader [{1}]" , model.model_name , model.shader_name);
            model.corrupted = true;
            EXIT_FUNCTION_TRACE();
            return;
        }
        
        EXIT_FUNCTION_TRACE();
    }

    void Systems::PhysicsBodyCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& transform = registry.get<components::Transform>(entity);
        auto& body = registry.get<components::PhysicsBody>(entity);

        body.body = PhysicsEngine::Instance()->CreateRigidBody(transform);
        switch (body.type) {
            case PhysicsBodyType::STATIC: body.body->setType(reactphysics3d::BodyType::STATIC); break;
            case PhysicsBodyType::KINEMATIC: body.body->setType(reactphysics3d::BodyType::KINEMATIC); break;
            case PhysicsBodyType::DYNAMIC: body.body->setType(reactphysics3d::BodyType::DYNAMIC); break;
            default:
                ENGINE_ERROR("UNDEFINED BEHAVIOR | UNREACHABLE CODE");
        }

        EXIT_FUNCTION_TRACE();
    }

    void Systems::BoxColliderCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& transform = registry.get<components::Transform>(entity);
        auto& collider = registry.get<components::BoxCollider>(entity);
        auto& body = registry.get<components::PhysicsBody>(entity);

        collider.shape = PhysicsEngine::Instance()->CreateBoxShape(transform);
        rp3d::Transform local_transform = reactphysics3d::Transform::identity();

        collider.collider = body.body->addCollider(collider.shape , local_transform);

        EXIT_FUNCTION_TRACE();
    }

    void Systems::SphereColliderCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = registry.get<components::SphereCollider>(entity);
        auto& body = registry.get<components::PhysicsBody>(entity);

        collider.shape = PhysicsEngine::Instance()->CreateSphereShape(collider.radius);
        rp3d::Transform local_transform = reactphysics3d::Transform::identity();

        collider.collider = body.body->addCollider(collider.shape , local_transform);

        EXIT_FUNCTION_TRACE();
    }

    void Systems::CapsuleColliderCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = registry.get<components::CapsuleCollider>(entity);
        auto& body = registry.get<components::PhysicsBody>(entity);

        collider.shape = PhysicsEngine::Instance()->CreateCapsuleShape(collider.radius , collider.height);
        rp3d::Transform local_transform = reactphysics3d::Transform::identity();

        collider.collider = body.body->addCollider(collider.shape , local_transform);

        EXIT_FUNCTION_TRACE();
    }

    void Systems::MeshColliderCreated(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& transform = registry.get<components::Transform>(entity);
        auto& model = registry.get<components::RenderableModel>(entity);
        auto& collider = registry.get<components::MeshCollider>(entity);
        auto& body = registry.get<components::PhysicsBody>(entity);

        PhysicsEngine* physics_engine = PhysicsEngine::Instance();
        collider.mesh = physics_engine->CreatePolygonMesh(model.model->Vertices() , model.model->Indices() , model.model->NumFaces());
        collider.shape = physics_engine->CreateConvexMeshShape(collider.mesh);
        rp3d::Transform local_transform = reactphysics3d::Transform::identity();

        collider.collider = body.body->addCollider(collider.shape , local_transform);

        EXIT_FUNCTION_TRACE();
    } 

    void Systems::LoadShaders(Scene* context) {
        ENTER_FUNCTION_TRACE();

        auto& registry = context->registry;

        registry.view<components::ID , components::Renderable>().each([](auto& id , auto& renderable) {
            LoadShader(renderable.shader , id.name , renderable.shader_name , renderable.corrupted);
        });

        registry.view<components::ID , components::TexturedRenderable>().each([](auto& id , auto& renderable) {
            LoadShader(renderable.shader , id.name , renderable.shader_name , renderable.corrupted);
        });

        registry.view<components::ID , components::RenderableModel>().each([](auto& id , auto& script) {
            LoadShader(script.shader , id.name , script.shader_name , script.corrupted);
        });

        EXIT_FUNCTION_TRACE();
    }
            
    void Systems::BindScripts(Scene* context) {
        ENTER_FUNCTION_TRACE();

        ScriptEngine* script_engine = ScriptEngine::Instance();
        auto& registry = context->registry;

        registry.view<components::Script>().each([script_engine](auto& script) {
            script.Bind(script.class_name);
        });

        EXIT_FUNCTION_TRACE();
    }

    void Systems::UpdateScene(Scene* context , float dt) {
        ENTER_FUNCTION_TRACE();

        PhysicsEngine::Instance()->StepPhysics();

        if (ResourceHandler::Instance()->ShadersReloaded()) {
            LoadShaders(context);
            ResourceHandler::Instance()->AcknowledgeShaderReload();
        }

        if (ScriptEngine::Instance()->ModulesReloaded()) {
            BindScripts(context);
            ScriptEngine::Instance()->AcknowledgeModuleReload();
        }

        EXIT_FUNCTION_TRACE();
    }

    void Systems::UpdateTransform(components::Transform& transform) {
        ENTER_FUNCTION_TRACE();

        transform.model = glm::mat4(1.f);
        transform.model = glm::translate(transform.model , transform.position);
        transform.model = glm::rotate(transform.model , transform.rotation.x , glm::vec3(1.f , 0.f , 0.f));
        transform.model = glm::rotate(transform.model , transform.rotation.y , glm::vec3(0.f , 1.f , 0.f));
        transform.model = glm::rotate(transform.model , transform.rotation.z , glm::vec3(0.f , 0.f , 1.f));
        transform.model = glm::scale(transform.model , transform.scale);

        EXIT_FUNCTION_TRACE();
    }
 
    void Systems::UpdatePhysicsBody(components::PhysicsBody& body , components::Transform& transform) {
        ENTER_FUNCTION_TRACE();

        const rp3d::Transform& physics_transform = body.body->getTransform();
        const rp3d::Transform& inter_transform = rp3d::Transform::interpolateTransforms(
            body.GetInterpolationTransform() , physics_transform , PhysicsEngine::Instance()->Alpha()
        );

        rp3d::decimal new_model_mat[kSizeOfTransformMat];
        inter_transform.getOpenGLMatrix(new_model_mat);

        transform.model = glm::mat4(1.f);
        transform.model = glm::make_mat4(new_model_mat);
        transform.model = glm::scale(transform.model , transform.scale);

        transform.rotation.y = asin(-transform.model[0][2]);
		if (cos(transform.rotation.y) != 0) {
		    transform.rotation.x = atan2(transform.model[1][2], transform.model[2][2]);
		    transform.rotation.z = atan2(transform.model[0][1], transform.model[0][0]);
		} else {
		    transform.rotation.x = atan2(-transform.model[2][0], transform.model[1][1]);
		    transform.rotation.z = 0;
		}

        transform.position = glm::vec3{
            physics_transform.getPosition().x , 
            physics_transform.getPosition().y , 
            physics_transform.getPosition().z
        };

        body.SetInterpolationTransform(physics_transform);

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::UpdateRenderable(components::Renderable& renderable , const std::vector<components::PointLight>& lights) {
        ENTER_FUNCTION_TRACE();

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::UpdateTexturedRenderable(components::TexturedRenderable& renderable , const std::vector<components::PointLight>& plights) {
        ENTER_FUNCTION_TRACE();

        if (renderable.corrupted || renderable.shader == nullptr) { 
            EXIT_FUNCTION_TRACE();
            return; 
        }

        Shader* shader = renderable.shader;

        renderable.shader->Bind();
        renderable.shader->SetUniformInt("point_light_count" ,  plights.size());
        
        for (uint32_t i = 0; i < plights.size(); ++i) {
            shader->SetUniformVec3("plights[" + std::to_string(i) + "].position" , plights[i].position);
            shader->SetUniformVec3("plights[" + std::to_string(i) + "].diffuse" , plights[i].diffuse);
            shader->SetUniformVec3("plights[" + std::to_string(i) + "].ambient" , plights[i].ambient);
            shader->SetUniformVec3("plights[" + std::to_string(i) + "].specular" , plights[i].specular);
            shader->SetUniformFloat("plights[" + std::to_string(i) + "].constant" , plights[i].constant_attenuation);
            shader->SetUniformFloat("plights[" + std::to_string(i) + "].linear" , plights[i].linear_attenuation);
            shader->SetUniformFloat("plights[" + std::to_string(i) + "].quadratic" , plights[i].quadratic_attenuation);
        }
        renderable.shader->SetUniformFloat("material.shininess" , renderable.material.shininess);
        renderable.shader->Unbind();

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::UpdateRenderableModel(components::RenderableModel& renderable , const std::vector<components::PointLight>& lights) {
        ENTER_FUNCTION_TRACE();


        EXIT_FUNCTION_TRACE();
    }

    void Systems::UnbindScripts(Scene* context) {
        ENTER_FUNCTION_TRACE();

        ScriptEngine* script_engine = ScriptEngine::Instance();
        auto& registry = context->registry;

        registry.view<components::Script>().each([script_engine](auto& script) {
            if (script.bound)
                script.Unbind();
        });

        EXIT_FUNCTION_TRACE();
    }

    void Systems::EntityDestroyed(Scene* context , Entity* entity) {
        ENTER_FUNCTION_TRACE();

        auto& id = entity->GetComponent<components::ID>();

        if (entity->HasComponent<components::Script>()) {
            ScriptEngine* script_engine = ScriptEngine::Instance();
            
            auto& script = entity->GetComponent<components::Script>();

            script_engine->DeactivateEntity(entity);
            script_engine->DestroyEntity(entity);

            entity->RemoveComponent<components::Script>();
        }

        if (entity->HasComponent<components::NativeScript>()) {
            auto& script = entity->GetComponent<components::NativeScript>();
            script.Unbind();

            entity->RemoveComponent<components::NativeScript>();
        }

        if (entity->HasComponent<components::BoxCollider>())
            entity->RemoveComponent<components::BoxCollider>();

        if (entity->HasComponent<components::SphereCollider>())
            entity->RemoveComponent<components::SphereCollider>();

        if (entity->HasComponent<components::CapsuleCollider>())
            entity->RemoveComponent<components::CapsuleCollider>();

        if (entity->HasComponent<components::PhysicsBody>()) 
            entity->RemoveComponent<components::PhysicsBody>();

        Entity* parent = entity->GetParent();
    }

    void Systems::ModelDestroyed(entt::registry& registry , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& model = registry.get<components::RenderableModel>(entity);
        model.model = nullptr;
        model.shader = nullptr;

        EXIT_FUNCTION_TRACE();
    }

    void Systems::PhysicsBodyDestroyed(entt::registry& context , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& body = context.get<components::PhysicsBody>(entity);
        PhysicsEngine::Instance()->DestroyRigidBody(body.body);

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::BoxColliderDestroyed(entt::registry& context , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = context.get<components::BoxCollider>(entity);
        PhysicsEngine::Instance()->DestroyBoxShape(collider.shape);

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::SphereColliderDestroyed(entt::registry& context , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = context.get<components::SphereCollider>(entity);
        PhysicsEngine::Instance()->DestroySphereShape(collider.shape);

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::CapsuleColliderDestroyed(entt::registry& context , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = context.get<components::CapsuleCollider>(entity);
        PhysicsEngine::Instance()->DestroyCapsuleShape(collider.shape);

        EXIT_FUNCTION_TRACE();
    }
    
    void Systems::MeshColliderDestroyed(entt::registry& context , entt::entity entity) {
        ENTER_FUNCTION_TRACE();

        auto& collider = context.get<components::MeshCollider>(entity);
        PhysicsEngine::Instance()->DestroyConvexMeshShape(collider.shape);

        EXIT_FUNCTION_TRACE();
    }

    void Systems::SceneUnload(Scene* context) {
        ENTER_FUNCTION_TRACE();

        PhysicsEngine* physics_engine = PhysicsEngine::Instance();

        auto& registry = context->registry;
        registry.view<components::PhysicsBody>().each([physics_engine](auto& body) {
            physics_engine->DestroyRigidBody(body.body);
        });

        registry.view<components::BoxCollider>().each([physics_engine](auto& collider) {
            physics_engine->DestroyBoxShape(collider.shape);
        });

        registry.view<components::SphereCollider>().each([physics_engine](auto& collider) {
            physics_engine->DestroySphereShape(collider.shape);
        });

        registry.view<components::CapsuleCollider>().each([physics_engine](auto& collider) {
            physics_engine->DestroyCapsuleShape(collider.shape);
        });

        registry.view<components::MeshCollider>().each([physics_engine](auto& collider) {
            physics_engine->DestroyConvexMeshShape(collider.shape);
        });

        EXIT_FUNCTION_TRACE();
    }

    void Systems::ScenePlay(Scene* context) {
        ENTER_FUNCTION_TRACE();

        ScriptEngine::Instance()->StartScene();

        auto& registry = context->registry;
        registry.view<components::Transform , components::PhysicsBody>().each([](auto& transform , auto& body) {
            const rp3d::Transform& physics_transform = body.body->getTransform();
            rp3d::Transform starting_transform = physics_transform;
            starting_transform.setPosition({ transform.position.x , transform.position.y , transform.position.z });

            body.body->setTransform(starting_transform);
        });

        EXIT_FUNCTION_TRACE();
    }

    void Systems::StopScene(Scene* context) {
        ENTER_FUNCTION_TRACE();

        ScriptEngine::Instance()->StopScene();

        EXIT_FUNCTION_TRACE();
    }

    void Systems::CleanupContext(Scene* context) {
        ENTER_FUNCTION_TRACE();

        auto& registry = context->registry;
        registry.on_destroy<components::MeshCollider>().disconnect<&MeshColliderDestroyed>();
        registry.on_destroy<components::CapsuleCollider>().disconnect<&CapsuleColliderDestroyed>();
        registry.on_destroy<components::SphereCollider>().disconnect<&SphereColliderDestroyed>();
        registry.on_destroy<components::BoxCollider>().disconnect<&BoxColliderDestroyed>();
        registry.on_destroy<components::PhysicsBody>().disconnect<&PhysicsBodyDestroyed>();
        registry.on_destroy<components::RenderableModel>().disconnect<&ModelDestroyed>();

        registry.on_construct<components::MeshCollider>().disconnect<&MeshColliderCreated>();
        registry.on_construct<components::CapsuleCollider>().disconnect<&CapsuleColliderCreated>();
        registry.on_construct<components::SphereCollider>().disconnect<&SphereColliderCreated>();
        registry.on_construct<components::BoxCollider>().disconnect<&BoxColliderCreated>();
        registry.on_construct<components::PhysicsBody>().disconnect<&PhysicsBodyCreated>();
        registry.on_construct<components::RenderableModel>().disconnect<&ModelCreated>();
        registry.on_construct<entt::entity>().disconnect<&EntityConstructed>();

        EXIT_FUNCTION_TRACE();
    }

    void Systems::Teardown() {
        ENTER_FUNCTION_TRACE();

        physics_body_update_sink.disconnect<&UpdatePhysicsBody>();
        update_entity_sink.disconnect<&UpdateTransform>();
        update_sink.disconnect<&UpdateScene>();
        scene_stop_sink.disconnect<&StopScene>();
        scene_start_sink.disconnect<&ScenePlay>();
        scene_unload_sink.disconnect<&SceneUnload>();
        scene_load_sink.disconnect<&BindScripts>();
        scene_load_sink.disconnect<&LoadShaders>();
        destroy_entity_sink.disconnect<&EntityDestroyed>();
        create_entity_sink.disconnect<&EntityCreated>();

        EXIT_FUNCTION_TRACE();
    }

}