#include "scripting/script_glue.hpp"

#include <mono/utils/mono-publib.h>

#include "scripting/script_engine.hpp"
#include "log.hpp"
#include "core/types.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "rendering/camera.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "scene/systems.hpp"
#include "physics/physics_engine.hpp"

namespace YE {

    static FunctionMaps* func_map = nullptr;

    template <typename T>
    std::unordered_map<MonoType* , std::function<bool(Entity* , T*)>> FunctionMaps::component_getters{};

    template <typename T>
    std::unordered_map<MonoType* , std::function<void(Entity* , T*)>> FunctionMaps::component_setters{};
    
    template<typename T>
    static void RegisterType() {
        std::string_view name = TypeData<T , true>().Name();

        if (name.find("components::") == std::string_view::npos) {
            ENGINE_ERROR("Attempted to register type that is not a component: {0}" , name);
            return;
        } else {
            size_t components = name.find("components::");
            name.remove_prefix(components + std::strlen("components::"));
        }

        std::string full_name = "YE." + std::string(name);

        MonoType* type = mono_reflection_type_from_name(full_name.data() , ScriptEngine::Instance()->GetInternalScriptData()->image);
        YE_CRITICAL_ASSERTION(type != nullptr , "Failed to get type from name: " + full_name);

        func_map->component_builders[type] = [](Entity* entity) { entity->AddComponent<T>(); };
        func_map->component_checkers[type] = [](Entity* entity) { return entity->HasComponent<T>(); };
        func_map->component_destroyers[type] = [](Entity* entity) { entity->RemoveComponent<T>(); };

        func_map->component_getters<T>[type] = [](Entity* entity , T* component) -> bool {
            if (!entity->HasComponent<T>()) {
                ENGINE_ERROR(
                    "Attempted to get component {0} from entity {1} that does not have it" , 
                    TypeData<T , true>().Name() , 
                    entity->GetComponent<components::ID>().name
                );

                *component = T();
                return false;
            }

            const auto& ent_component = entity->GetComponent<T>();
            *component = ent_component;
            return true;
        };

        func_map->component_setters<T>[type] = [](Entity* entity , T* component) {
            if (!entity->HasComponent<T>()) {
                ENGINE_ERROR(
                    "Attempted to set component {0} from entity {1} that does not have it" , 
                    TypeData<T , true>().Name() , 
                    entity->GetComponent<components::ID>().name
                );

                return;
            }

            if (component == nullptr) {
                ENGINE_ERROR("Attempted to set component {0} to null" , TypeData<T , true>().Name());
                return;
            }

            auto& ent_component = entity->GetComponent<T>();
            ent_component = *component;
        };
    }
    
    void ScriptGlue::BindTypes() {
        RegisterType<components::ID>();
        RegisterType<components::Transform>();
        RegisterType<components::Renderable>();
        RegisterType<components::TexturedRenderable>();
        RegisterType<components::RenderableModel>();
        RegisterType<components::DirectionalLight>();
        RegisterType<components::PointLight>();
        RegisterType<components::SpotLight>();
        RegisterType<components::PhysicsBody>();
        RegisterType<components::Script>();
        RegisterType<components::NativeScript>();
    }

    void ScriptGlue::BindMethods() {
        YE_ADD_SCRIPT_FUNCTION(EntityNameFromId);
        YE_ADD_SCRIPT_FUNCTION(SetEntityName);

        YE_ADD_SCRIPT_FUNCTION(GetEntityTransform);
        YE_ADD_SCRIPT_FUNCTION(SetEntityTransform);
        YE_ADD_SCRIPT_FUNCTION(GetEntityPosition);
        YE_ADD_SCRIPT_FUNCTION(SetEntityPosition);
        YE_ADD_SCRIPT_FUNCTION(GetEntityScale);
        YE_ADD_SCRIPT_FUNCTION(SetEntityScale);
        YE_ADD_SCRIPT_FUNCTION(GetEntityRotation);
        YE_ADD_SCRIPT_FUNCTION(SetEntityRotation);

        YE_ADD_SCRIPT_FUNCTION(GetPhysicsBodyType);
        YE_ADD_SCRIPT_FUNCTION(SetPhysicsBodyType);
        YE_ADD_SCRIPT_FUNCTION(GetPhysicsBodyPosition);
        YE_ADD_SCRIPT_FUNCTION(SetPhysicsBodyPosition);
        YE_ADD_SCRIPT_FUNCTION(GetPhysicsBodyRotation);
        YE_ADD_SCRIPT_FUNCTION(SetPhysicsBodyRotation);
        YE_ADD_SCRIPT_FUNCTION(GetPhysicsBodyMass);
        YE_ADD_SCRIPT_FUNCTION(SetPhysicsBodyMass);
        YE_ADD_SCRIPT_FUNCTION(ApplyForceCenterOfMass);
        YE_ADD_SCRIPT_FUNCTION(ApplyForce);
        YE_ADD_SCRIPT_FUNCTION(ApplyTorque);

        YE_ADD_SCRIPT_FUNCTION(Mat4LookAt);

        YE_ADD_SCRIPT_FUNCTION(GetCameraType);     
        YE_ADD_SCRIPT_FUNCTION(SetCameraType);
        
        YE_ADD_SCRIPT_FUNCTION(RecalculateCameraViewProjectionMatrix);
        YE_ADD_SCRIPT_FUNCTION(GetCameraView);
        YE_ADD_SCRIPT_FUNCTION(GetCameraProjection);
        YE_ADD_SCRIPT_FUNCTION(GetCameraViewProjection);
        YE_ADD_SCRIPT_FUNCTION(GetCameraPosition);
        YE_ADD_SCRIPT_FUNCTION(SetCameraPosition);
        YE_ADD_SCRIPT_FUNCTION(GetCameraFront);
        YE_ADD_SCRIPT_FUNCTION(SetCameraFront);
        YE_ADD_SCRIPT_FUNCTION(GetCameraUp);
        YE_ADD_SCRIPT_FUNCTION(SetCameraUp);
        YE_ADD_SCRIPT_FUNCTION(GetCameraRight);
        YE_ADD_SCRIPT_FUNCTION(GetCameraWorldUp);
        YE_ADD_SCRIPT_FUNCTION(SetCameraWorldUp);
        YE_ADD_SCRIPT_FUNCTION(GetCameraEulerAngles);
        YE_ADD_SCRIPT_FUNCTION(SetCameraEulerAngles);
        YE_ADD_SCRIPT_FUNCTION(GetCameraViewportSize);
        YE_ADD_SCRIPT_FUNCTION(SetCameraViewportSize);
        YE_ADD_SCRIPT_FUNCTION(GetCameraClip);
        YE_ADD_SCRIPT_FUNCTION(SetCameraClip);
        YE_ADD_SCRIPT_FUNCTION(GetCameraMousePos);
        YE_ADD_SCRIPT_FUNCTION(GetCameraLastMousePos);
        YE_ADD_SCRIPT_FUNCTION(GetCameraDeltaMouse);
        YE_ADD_SCRIPT_FUNCTION(GetCameraSpeed);
        YE_ADD_SCRIPT_FUNCTION(SetCameraSpeed);
        YE_ADD_SCRIPT_FUNCTION(GetCameraSensitivity);
        YE_ADD_SCRIPT_FUNCTION(SetCameraSensitivity);
        YE_ADD_SCRIPT_FUNCTION(GetCameraFov);
        YE_ADD_SCRIPT_FUNCTION(SetCameraFov);
        YE_ADD_SCRIPT_FUNCTION(GetCameraZoom);
        YE_ADD_SCRIPT_FUNCTION(SetCameraZoom);
        YE_ADD_SCRIPT_FUNCTION(GetCameraConstrainPitch);
        YE_ADD_SCRIPT_FUNCTION(SetCameraConstrainPitch);
        YE_ADD_SCRIPT_FUNCTION(IsCameraPrimary);
        YE_ADD_SCRIPT_FUNCTION(SetCameraPrimary);

        YE_ADD_SCRIPT_FUNCTION(EntityAddComponent);
        YE_ADD_SCRIPT_FUNCTION(EntityHasComponent);
        YE_ADD_SCRIPT_FUNCTION(EntityRemoveComponent);
        YE_ADD_SCRIPT_FUNCTION(GetEntityParent);
        YE_ADD_SCRIPT_FUNCTION(SetEntityParent);

        YE_ADD_SCRIPT_FUNCTION(CreateEntity);
        YE_ADD_SCRIPT_FUNCTION(DestroyEntity);
        YE_ADD_SCRIPT_FUNCTION(IsEntityValid);
        YE_ADD_SCRIPT_FUNCTION(GetEntityByName);

        YE_ADD_SCRIPT_FUNCTION(KeyFramesHeld);
        YE_ADD_SCRIPT_FUNCTION(IsKeyPressed);
        YE_ADD_SCRIPT_FUNCTION(IsKeyBlocked);
        YE_ADD_SCRIPT_FUNCTION(IsKeyHeld);
        YE_ADD_SCRIPT_FUNCTION(IsKeyDown);
        YE_ADD_SCRIPT_FUNCTION(IsKeyReleased);

        YE_ADD_SCRIPT_FUNCTION(SnapMouseToCenter);
        YE_ADD_SCRIPT_FUNCTION(FreeMouse);
        YE_ADD_SCRIPT_FUNCTION(HideMouse);
        YE_ADD_SCRIPT_FUNCTION(MouseX);
        YE_ADD_SCRIPT_FUNCTION(MouseY);
        YE_ADD_SCRIPT_FUNCTION(MouseLastX);
        YE_ADD_SCRIPT_FUNCTION(MouseLastY);
        YE_ADD_SCRIPT_FUNCTION(MouseDX);
        YE_ADD_SCRIPT_FUNCTION(MouseDY);
        YE_ADD_SCRIPT_FUNCTION(MousePos);
        YE_ADD_SCRIPT_FUNCTION(MouseLastPos);
        YE_ADD_SCRIPT_FUNCTION(MouseDelta);
        YE_ADD_SCRIPT_FUNCTION(MouseInWindow);
        YE_ADD_SCRIPT_FUNCTION(ButtonFramesHeld);
        YE_ADD_SCRIPT_FUNCTION(IsMouseButtonPressed);
        YE_ADD_SCRIPT_FUNCTION(IsMouseButtonBlocked);
        YE_ADD_SCRIPT_FUNCTION(IsMouseButtonHeld);
        YE_ADD_SCRIPT_FUNCTION(IsMouseButtonReleased);

        YE_ADD_SCRIPT_FUNCTION(LogTrace);
        YE_ADD_SCRIPT_FUNCTION(LogDebug);
        YE_ADD_SCRIPT_FUNCTION(LogInfo);
        YE_ADD_SCRIPT_FUNCTION(LogWarn);
        YE_ADD_SCRIPT_FUNCTION(LogError);
    }
    
    void ScriptGlue::BindAssembly() {
        YE_CRITICAL_ASSERTION(func_map == nullptr , "Attempted to bind assembly when it is already bound");
        func_map = ynew FunctionMaps();

        BindTypes();
        BindMethods();
    }

    
    void ScriptGlue::UnbindAssembly() {
        if (func_map != nullptr) {
            if (func_map->component_builders.size() > 0)
                func_map->component_builders.clear();
            
            if (func_map->component_checkers.size() > 0)
                func_map->component_checkers.clear();

            if (func_map->component_destroyers.size() > 0)
                func_map->component_destroyers.clear();

            ydelete func_map;
            func_map = nullptr;
        }
    }

namespace ScriptInternalCalls {

    // ********* Tag Functions ********* //
    MonoString* EntityNameFromId(uint64_t entity_id) {
        return nullptr;
    }

    void SetEntityName(uint64_t entity_id , MonoString* name) {

    }
    // ******************************** //

    // ****** Grouping Functions ******* //
    // ********************************* //

    // ****** Transform Functions ******* //
    void GetEntityTransform(uint64_t entity_id , components::Transform* transform) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetEntityTransform :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            *transform = components::Transform();
            return;
        }

        const auto& ent_transform = entity->GetComponent<components::Transform>();
        transform->position = ent_transform.position;
        transform->rotation = ent_transform.rotation;
        transform->scale = ent_transform.scale;
    }

    void SetEntityTransform(uint64_t entity_id , components::Transform* transform) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);

        if (entity == nullptr) {
            ENGINE_ERROR("SetEntityTransform :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (transform == nullptr) {
            ENGINE_ERROR("SetEntityTransform :: Attempted to set transform to null");
            return;
        }

        auto& ent_transform = entity->GetComponent<components::Transform>();

        ent_transform.position = transform->position;
        ent_transform.rotation = transform->rotation;
        ent_transform.scale = transform->scale;

        if (entity->HasComponent<components::PhysicsBody>()) {
            auto& body = entity->GetComponent<components::PhysicsBody>();
            rp3d::Transform t = body.body->getTransform();
            t.setPosition(
                rp3d::Vector3(transform->position.x , transform->position.y , transform->position.z)
            );
            t.setOrientation(
                rp3d::Quaternion::fromEulerAngles(
                    transform->rotation.x , transform->rotation.y , transform->rotation.z
                )
            ); 
            body.body->setTransform(t);
        }


        /// \todo figure out why this screws up the size of the physics engine collider arrays and causes a crash on shutdown
        // if (entity->HasComponent<components::BoxCollider>()) {
        //     entity->RemoveComponent<components::BoxCollider>();
        //     entity->AddComponent<components::BoxCollider>();
        // }
    }

    void GetEntityPosition(uint64_t entity_id , glm::vec3* position) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetEntityPosition :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (position == nullptr) {
            ENGINE_ERROR("GetEntityPosition :: Attempted to retrieve position with null pointer");
            return;
        }

        const auto& ent_transform = entity->GetComponent<components::Transform>();
        *position = ent_transform.position;
    }

    void SetEntityPosition(uint64_t entity_id , glm::vec3* position) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);

        if (entity == nullptr) {
            ENGINE_ERROR("SetEntityPosition :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (position == nullptr) {
            ENGINE_ERROR("SetEntityPosition :: Attempted to set position to null");
            return;
        }

        auto& ent_transform = entity->GetComponent<components::Transform>();
        ent_transform.position = *position;

        if (entity->HasComponent<components::PhysicsBody>()) {
            auto& body = entity->GetComponent<components::PhysicsBody>();
            rp3d::Transform t = body.body->getTransform();
            t.setPosition(
                rp3d::Vector3(position->x , position->y , position->z)
            );
            body.body->setTransform(t);
        }
    }

    void GetEntityRotation(uint64_t entity_id , glm::vec3* rotation) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetEntityRotation :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        const auto& ent_transform = entity->GetComponent<components::Transform>();
        *rotation = ent_transform.rotation;
    }

    void SetEntityRotation(uint64_t entity_id , glm::vec3* rotation) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetEntityRotation :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (rotation == nullptr) {
            ENGINE_ERROR("SetEntityRotation :: Attempted to set rotation to null");
            return;
        }

        auto& ent_transform = entity->GetComponent<components::Transform>();
        ent_transform.rotation = *rotation;

        if (entity->HasComponent<components::PhysicsBody>()) {
            auto& body = entity->GetComponent<components::PhysicsBody>();
            rp3d::Transform t = body.body->getTransform();
            t.setOrientation(
                rp3d::Quaternion::fromEulerAngles(
                    rotation->x , rotation->y , rotation->z
                )
            );
            body.body->setTransform(t);
        }
    }

    void GetEntityScale(uint64_t entity_id , glm::vec3* scale) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetEntityScale :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        const auto& ent_transform = entity->GetComponent<components::Transform>();
        *scale = ent_transform.scale;
    }

    void SetEntityScale(uint64_t entity_id , glm::vec3* scale) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetEntityScale :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (scale == nullptr) {
            ENGINE_ERROR("SetEntityScale :: Attempted to set scale to null");
            return;
        }

        auto& ent_transform = entity->GetComponent<components::Transform>();
        ent_transform.scale = *scale;

        /// \todo figure out why this screws up the size of the physics engine collider arrays and causes a crash on shutdown
        // if (entity->HasComponent<components::BoxCollider>()) {
        //     entity->RemoveComponent<components::BoxCollider>();
        //     entity->AddComponent<components::BoxCollider>();
        // }
    }
    // ********************************** //

    // ********* Renderable Functions ********* //
    // **************************************** //

    // ********* TexturedRenderable Functions ********* //
    // ************************************************ //

    // ********* CubeMapRenderable Functions ********* //
    // *********************************************** //

    // ********* RenderableModel Functions ********* //
    // ********************************************* //

    // ********* DirectionalLight Functions ********* //
    // ********************************************** //

    // ********* PointLight Functions ********* //
    // **************************************** //

    // ********* SpotLight Functions ********* //
    // *************************************** //

    // ********* PhysicsBody Functions ********* //
    uint32_t GetPhysicsBodyType(uint64_t entity_id) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr)  {
            ENGINE_ERROR("GetPhysicsBodyType :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return 0;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("GetPhysicsBodyType :: Attempted to retrieve physics body from entity that does not have it");
            return 0;
        }

        const auto& body = entity->GetComponent<components::PhysicsBody>();
        return static_cast<uint32_t>(body.type);
    }

    void SetPhysicsBodyType(uint64_t entity_id , uint32_t type) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyType :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("SetPhysicsBodyType :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (type > static_cast<uint32_t>(PhysicsBodyType::DYNAMIC)) {
            ENGINE_ERROR("SetPhysicsBodyType :: Attempted to set physics body type to invalid type: {0}" , type);
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.type = static_cast<PhysicsBodyType>(type);
        body.body->setType(static_cast<rp3d::BodyType>(body.type));
    }

    void GetPhysicsBodyPosition(uint64_t entity_id , glm::vec3* position) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetPhysicsBodyPosition :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("GetPhysicsBodyPosition :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        const auto& body = entity->GetComponent<components::PhysicsBody>();
        rp3d::Vector3 pos = body.body->getTransform().getPosition();

        *position = glm::vec3(pos.x , pos.y , pos.z);
    }

    void SetPhysicsBodyPosition(uint64_t entity_id , glm::vec3* position) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyPosition :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("SetPhysicsBodyPosition :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (position == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyPosition :: Attempted to set physics body position to null");
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->setTransform(
            rp3d::Transform(
                rp3d::Vector3(position->x , position->y , position->z) , 
                    body.body->getTransform().getOrientation()
            )
        );
    }

    void GetPhysicsBodyRotation(uint64_t entity_id , glm::vec3* rotation) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetPhysicsBodyRotation :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("GetPhysicsBodyRotation :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        const auto& body = entity->GetComponent<components::PhysicsBody>();
        rp3d::Quaternion rot = body.body->getTransform().getOrientation();

        float x = rot.x;
        float y = rot.y;
        float z = rot.z;
        float w = rot.w;

        glm::vec3 euler;
        
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        euler.x = std::atan2(sinr_cosp , cosr_cosp);

        float sinp = glm::sqrt(1 + 2 * (w * y - z * x));
        float cosp = glm::sqrt(1 - 2 * (y * y + z * z));
        euler.y = 2 * std::atan2(sinp , cosp) - glm::radians(90.f);

        double siny_cosp = 2 * (w * z + x * y);
        double cosy_cosp = 1 - 2 * (y * y + z * z);
        euler.z = std::atan2(siny_cosp , cosy_cosp);

        *rotation = euler;
    }

    void SetPhysicsBodyRotation(uint64_t entity_id , glm::vec3* rotation) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyRotation :: Attempted to retrieve invalid entity from ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("SetPhysicsBodyRotation :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (rotation == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyRotation :: Attempted to set physics body rotation to null");
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->setTransform(
            rp3d::Transform(
                body.body->getTransform().getPosition() , 
                    rp3d::Quaternion::fromEulerAngles(
                        rotation->x , rotation->y , rotation->z
                    )
            )
        );
    }

    float GetPhysicsBodyMass(uint64_t entity_id) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("GetPhysicsBodyMass :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return 0.f;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("GetPhysicsBodyMass :: Attempted to retrieve physics body from entity that does not have it");
            return 0.f;
        }

        const auto& body = entity->GetComponent<components::PhysicsBody>();
        return body.body->getMass();
    }

    void SetPhysicsBodyMass(uint64_t entity_id , float mass) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("SetPhysicsBodyMass :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_ERROR("SetPhysicsBodyMass :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->setMass(mass);
    }
    
    void ApplyForceCenterOfMass(uint64_t entity_id , glm::vec3* force) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_WARN("ApplyForceCenterOfMass :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_WARN("ApplyForceCenterOfMass :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (force == nullptr) {
            ENGINE_WARN("ApplyForceCenterOfMass :: Attempted to apply force to null");
            return;
        }

        *force *= 1000.f;

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->applyWorldForceAtCenterOfMass(rp3d::Vector3(force->x , force->y , force->z));
    }
    
    void ApplyForce(uint64_t entity_id , glm::vec3* force , glm::vec3* point) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_WARN("ApplyForce :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_WARN("ApplyForce :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (force == nullptr) {
            ENGINE_WARN("ApplyForce :: Attempted to apply force to null");
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->applyLocalForceAtLocalPosition(
            rp3d::Vector3(force->x , force->y , force->z) , 
            rp3d::Vector3(point->x , point->y , point->z)
        );
    }
    
    void ApplyTorque(uint64_t entity_id , glm::vec3* torque) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_WARN("ApplyTorque :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        if (!entity->HasComponent<components::PhysicsBody>()) {
            ENGINE_WARN("ApplyTorque :: Attempted to retrieve physics body from entity that does not have it");
            return;
        }

        if (torque == nullptr) {
            ENGINE_WARN("ApplyTorque :: Attempted to apply force to null");
            return;
        }

        auto& body = entity->GetComponent<components::PhysicsBody>();
        body.body->applyWorldTorque(rp3d::Vector3(torque->x , torque->y , torque->z));
    }
    // ***************************************** //
        
    // ********* Math Functions ********* //
    void Mat4LookAt(glm::vec3* pos , glm::vec3* front , glm::vec3* up , glm::mat4* dest) {
        *dest = glm::lookAt(*pos , *pos + *front , *up);
    }
    // ********************************** //

    // ****** Camera Functions ******* //
    uint32_t GetCameraType(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraType :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return 0;
        }

        return static_cast<uint32_t>(camera->Type());
    }

    void SetCameraType(uint32_t camera_id, uint32_t type) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("SetCameraType :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (type > static_cast<uint32_t>(CameraType::ORTHOGRAPHIC)) {
            ENGINE_ERROR("SetCameraType :: Attempted to set camera type to invalid type: {0}" , type);
            return;
        }

        camera->SetType(static_cast<CameraType>(type));
    }

    void RecalculateCameraViewProjectionMatrix(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("RecalculateCameraViewProjectionMatrix :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->Recalculate();
    }

    void GetCameraView(uint32_t camera_id , glm::mat4* matrix) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraView :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *matrix = camera->View();
    }

    void GetCameraProjection(uint32_t camera_id , glm::mat4* matrix) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *matrix = camera->Projection();
    }

    void GetCameraViewProjection(uint32_t camera_id , glm::mat4* matrix) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *matrix = camera->ViewProjection();
    }

    void GetCameraPosition(uint32_t camera_id , glm::vec3* position) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraPosition :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *position = camera->Position();
    }

    void SetCameraPosition(uint32_t camera_id , glm::vec3* position) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (position == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetPosition(*position);
    }

    void GetCameraFront(uint32_t camera_id , glm::vec3* front) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraFront :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *front = camera->Front();
    }

    void SetCameraFront(uint32_t camera_id , glm::vec3* front) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (front == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetFront(*front);
    }

    void GetCameraUp(uint32_t camera_id , glm::vec3* up) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *up = camera->Up();
    }

    void SetCameraUp(uint32_t camera_id , glm::vec3* up) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (up == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetUp(*up);
    }

    void GetCameraRight(uint32_t camera_id , glm::vec3* right) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *right = camera->Right();
    }

    void GetCameraWorldUp(uint32_t camera_id , glm::vec3* world_up) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *world_up = camera->WorldUp();
    }

    void SetCameraWorldUp(uint32_t camera_id , glm::vec3* world_up) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (world_up == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetWorldUp(*world_up);
    }

    void GetCameraEulerAngles(uint32_t camera_id , glm::vec3* angles) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *angles = camera->Orientation();
    }

    void SetCameraEulerAngles(uint32_t camera_id , glm::vec3* angles) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }
        
        if (angles == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetOrientation(*angles);
    }

    void GetCameraViewportSize(uint32_t camera_id , glm::vec2* size) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        glm::ivec2 viewport = camera->Viewport(); 
        *size = glm::vec2(viewport.x , viewport.y);
    }

    void SetCameraViewportSize(uint32_t camera_id , glm::vec2* size) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        if (size == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        glm::ivec2 viewport = glm::ivec2(size->x , size->y);
        camera->SetViewport(viewport);
    }

    void GetCameraClip(uint32_t camera_id , glm::vec2* clip) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *clip = camera->Clip();
    }

    void SetCameraClip(uint32_t camera_id , glm::vec2* clip) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }
        
        if (clip == nullptr) {
            ENGINE_ERROR("SetCameraPosition :: Attempted to set camera position to null");
            return;
        }

        camera->SetClip(*clip);
    }

    void SetCameraNearClip(uint32_t camera_id , float near_clip) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("SetCameraNearClip :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        glm::vec2 clip = camera->Clip();
        clip.x = near_clip;
        camera->SetClip(clip);
    }

    void SetCameraFarClip(uint32_t camera_id , float far_clip) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("SetCameraFarClip :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        glm::vec2 clip = camera->Clip();
        clip.y = far_clip;
        camera->SetClip(clip);
    }

    void GetCameraMousePos(uint32_t camera_id , glm::vec2* position) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *position = camera->Mouse();
    }

    void GetCameraLastMousePos(uint32_t camera_id , glm::vec2* position) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *position = camera->LastMouse();
    }

    void GetCameraDeltaMouse(uint32_t camera_id , glm::vec2* delta) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        *delta = camera->DeltaMouse();
    }

    float GetCameraSpeed(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return 0.f;
        }

        return camera->Speed();
    }

    void SetCameraSpeed(uint32_t camera_id, float speed) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetSpeed(speed);
    }

    float GetCameraSensitivity(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return 0.f;
        }

        return camera->Sensitivity();
    }

    void SetCameraSensitivity(uint32_t camera_id , float sensitivity) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetSensitivity(sensitivity);
    }

    float GetCameraFov(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return 0.f;
        }

        return camera->FOV();
    }

    void SetCameraFov(uint32_t camera_id , float fov) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetFOV(fov);
    }

    float GetCameraZoom(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return 0.f;
        }

        return camera->Zoom();
    }

    void SetCameraZoom(uint32_t camera_id , float zoom) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetZoom(zoom);
    }

    bool GetCameraConstrainPitch(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return false;
        }

        return camera->ConstrainPitch();
    }

    void SetCameraConstrainPitch(uint32_t camera_id , bool constrain_pitch) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetConstrainPitch(constrain_pitch);
    }

    bool IsCameraPrimary(uint32_t camera_id) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return false;
        }

        return camera->IsPrimary();
    }

    void SetCameraPrimary(uint32_t camera_id , bool primary) {
        Camera* camera = ScriptEngine::Instance()->GetSceneContext()->GetCamera(camera_id);
        if (camera == nullptr) {
            ENGINE_ERROR("GetCameraViewProjection :: Attempted to retrieve invalid camera with ID: {0}" , camera_id);
            return;
        }

        camera->SetPrimary(primary);
    }
    // ******************************* //

    // ****** Entity Functions ******* //
    uint64_t GetEntityParent(uint64_t entity_id) {
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr)  {
            ENGINE_ERROR("GetEntityParent :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return 0;
        }

        Entity* parent = entity->GetParent();
        if (parent == nullptr) {
            ENGINE_ERROR("GetEntityParent :: Attempted to retrieve parent from entity that does not have one");
            return 0;
        }

        return parent->GetComponent<components::ID>().id.uuid;
    }

    void SetEntityParent(uint64_t child , uint64_t parent) {
        Entity* child_entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(child);
        if (child_entity == nullptr) {
            ENGINE_ERROR("SetEntityParent :: Attempted to retrieve invalid entity with ID: {0}", child);
            return;
        }

        if (parent == 0) {
            // remove parent
        } else {
            Entity* parent_entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(parent);
            if (parent_entity == nullptr) {
                ENGINE_ERROR("SetEntityParent :: Attempted to retrieve invalid entity with ID: {0}", parent);
                return;
            }

            child_entity->SetParent(parent_entity);
        }
    }

    void EntityAddComponent(uint64_t entity_id , MonoReflectionType* component_name) {
        YE::Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("EntityAddComponent :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        if (component_name == nullptr) {
            ENGINE_ERROR("EntityAddComponent :: Attempted to retrieve component with null name");
            mono_free(component_name);
            return;
        }

        MonoType* type = mono_reflection_type_get_type(component_name);
        if (type == nullptr) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityAddComponent :: Failed to get type from MonoReflectionType {0}" , component_name);
            mono_free(component_name);
            return;
        }

        if (func_map->component_builders.find(type) == func_map->component_builders.end()) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityAddComponent :: Failed to find component builder for type {0}" , component_name);
            mono_free(component_name);
            return;
        }

        func_map->component_builders[type](entity);
    }

    bool EntityHasComponent(uint64_t entity_id , MonoReflectionType* component_name) {
        YE::Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("EntityHasComponent :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return false;
        }

        if (component_name == nullptr) {
            ENGINE_ERROR("EntityHasComponent :: Attempted to retrieve component with null name");
            mono_free(component_name);
            return false;
        }

        MonoType* type = mono_reflection_type_get_type(component_name);
        if (type == nullptr) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityHasComponent :: Failed to get type from MonoReflectionType {0}" , component_name);
            mono_free(component_name);
            return false;
        }

        if (func_map->component_checkers.find(type) == func_map->component_checkers.end()) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityHasComponent :: Failed to find component checker for type {0}" , component_name);
            mono_free(component_name);
            return false;
        }

        return func_map->component_checkers[type](entity);
    }
    
    bool EntityRemoveComponent(uint64_t entity_id , MonoReflectionType* component_name) {
        YE::Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("EntityRemoveComponent :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return false;
        }

        if (component_name == nullptr) {
            ENGINE_ERROR("EntityRemoveComponent :: Attempted to retrieve component with null name");
            mono_free(component_name);
            return false;
        }

        MonoType* type = mono_reflection_type_get_type(component_name);
        if (type == nullptr) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityRemoveComponent :: Failed to get type from MonoReflectionType {0}" , component_name);
            mono_free(component_name);
            return false;
        }

        if (func_map->component_destroyers.find(type) == func_map->component_destroyers.end()) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityRemoveComponent :: Failed to find component destroyer for type {0}" , component_name);
            mono_free(component_name);
            return false;
        }

        if (!func_map->component_checkers[type](entity)) {
            char* component_name = mono_type_get_name(type);
            ENGINE_ERROR("EntityRemoveComponent :: Entity does not have component {0}" , component_name);
            mono_free(component_name);
            return false;
        }

        func_map->component_destroyers[type](entity);
        return true;
    }
    // ****************************** //

    // ****** Scene Functions ******* //
    bool IsEntityValid(uint64_t entity) {
        return ScriptEngine::Instance()->GetSceneContext()->IsEntityValid(entity);
    }
    
    uint64_t CreateEntity(MonoString* name) {
        char* str = mono_string_to_utf8(name);
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->CreateEntity(str);
        mono_free(str);
        return entity->GetComponent<components::ID>().id.uuid;
    }

    void DestroyEntity(uint64_t entity_id) {
        ScriptEngine* engine = ScriptEngine::Instance();
        
        if (!engine->GetSceneContext()->IsEntityValid(entity_id)) {
            ENGINE_ERROR("DestroyEntity :: Attempted to destroy invalid entity with ID: {0}" , entity_id);
            return;
        }

        Entity* entity = engine->GetSceneContext()->GetEntity(entity_id);
        if (entity == nullptr) {
            ENGINE_ERROR("DestroyEntity :: Attempted to retrieve invalid entity with ID: {0}" , entity_id);
            return;
        }

        engine->GetSceneContext()->DestroyEntity(entity);   
    }

    uint64_t GetEntityByName(MonoString* name) {
        char* str = mono_string_to_utf8(name);
        Entity* entity = ScriptEngine::Instance()->GetSceneContext()->GetEntity(str);
        mono_free(str);
        
        if (entity == nullptr) {
            ENGINE_ERROR("GetEntityByName :: Attempted to retrieve invalid entity with name: {0}" , str);
            return 0;
        }

        return entity->GetComponent<components::ID>().id.uuid;
    }
    // ****************************** //

    // *** Keyboard Functions *** //
    uint32_t KeyFramesHeld(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }

        Keyboard::KeyState state = Keyboard::GetKeyState(static_cast<Keyboard::Key>(key));
        return state.frames_held;
    }

    bool IsKeyPressed(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }

        return Keyboard::Pressed(static_cast<Keyboard::Key>(key));
    }

    bool IsKeyBlocked(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }
        
        return Keyboard::Blocked(static_cast<Keyboard::Key>(key));
    }

    bool IsKeyHeld(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }
        
        return Keyboard::Held(static_cast<Keyboard::Key>(key));
    }

    bool IsKeyDown(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }
        
        return Keyboard::KeyDown(static_cast<Keyboard::Key>(key));
    }

    bool IsKeyReleased(uint32_t key) {
        if (static_cast<Keyboard::Key>(key) < Keyboard::Key::YE_UNKNOWN || 
            static_cast<Keyboard::Key>(key) > Keyboard::Key::YE_NUM_YES) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , key);
            return false;
        }
        
        return Keyboard::Released(static_cast<Keyboard::Key>(key));
    }
    // ************************** //

    // *** Mouse Functions *** //
    void SnapMouseToCenter() {
        Mouse::SnapToCenter();
    }
    
    void FreeMouse() {
        Mouse::FreeCursor();
    }
    
    void HideMouse() {
        Mouse::HideCursor();
    }
    
    uint32_t MouseX() {
        return Mouse::X();
    }

    uint32_t MouseY() {
        return Mouse::Y();
    }

    uint32_t MouseLastX() {
        return Mouse::PreviousX();
    }

    uint32_t MouseLastY() {
        return Mouse::PreviousY();
    }

    uint32_t MouseDX() {
        return Mouse::DX();
    }

    uint32_t MouseDY() {
        return Mouse::DY();
    }

    void MousePos(glm::vec2* pos) {
        *pos = glm::vec2((float)Mouse::X() , (float)Mouse::Y());
    }

    void MouseLastPos(glm::vec2* pos) {
        *pos = glm::vec2((float)Mouse::PreviousX() , (float)Mouse::PreviousY());
    }

    void MouseDelta(glm::vec2* delta) {
        *delta = glm::vec2((float)Mouse::DX() , (float)Mouse::DY());
    }

    bool MouseInWindow() {
        return Mouse::InWindow();
    }

    uint32_t ButtonFramesHeld(uint32_t button) {
        if (static_cast<Mouse::Button>(button) < Mouse::Button::FIRST || 
            static_cast<Mouse::Button>(button) > Mouse::Button::LAST) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , button);
            return false;
        }

        Mouse::ButtonState state = Mouse::GetButtonState(static_cast<Mouse::Button>(button));
        return state.frames_held;
    }

    bool IsMouseButtonPressed(uint32_t button) {
        if (static_cast<Mouse::Button>(button) < Mouse::Button::FIRST || 
            static_cast<Mouse::Button>(button) > Mouse::Button::LAST) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , button);
            return false;
        }

        return Mouse::Pressed(static_cast<Mouse::Button>(button));
    }

    bool IsMouseButtonBlocked(uint32_t button) {
        if (static_cast<Mouse::Button>(button) < Mouse::Button::FIRST || 
            static_cast<Mouse::Button>(button) > Mouse::Button::LAST) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , button);
            return false;
        }

        return Mouse::Blocked(static_cast<Mouse::Button>(button));
    }

    bool IsMouseButtonHeld(uint32_t button) {
        if (static_cast<Mouse::Button>(button) < Mouse::Button::FIRST || 
            static_cast<Mouse::Button>(button) > Mouse::Button::LAST) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , button);
            return false;
        }

        return Mouse::Held(static_cast<Mouse::Button>(button));
    }

    bool IsMouseButtonReleased(uint32_t button) {
        if (static_cast<Mouse::Button>(button) < Mouse::Button::FIRST || 
            static_cast<Mouse::Button>(button) > Mouse::Button::LAST) {
            ENGINE_ERROR("IsKeyDown :: Attempted to retrieve invalid key: {0}" , button);
            return false;
        }

        return Mouse::Released(static_cast<Mouse::Button>(button));
    }
    // ************************ //

    // *** Logging Functions *** //
    void LogTrace(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        LOG_TRACE("[C#] {0}" , str);
        mono_free(str);
    }

    void LogDebug(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        LOG_DEBUG("[C#] {0}" , str);
        mono_free(str);
    }

    void LogInfo(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        LOG_INFO("[C#] {0}" , str);
        mono_free(str);
    }

    void LogWarn(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        LOG_WARN("[C#] {0}" , str);
        mono_free(str);
    }

    void LogError(MonoString* msg) {
        char* str = mono_string_to_utf8(msg);
        LOG_ERROR("[C#] {0}" , str);
        mono_free(str);
    }
    // ************************ //

}

}