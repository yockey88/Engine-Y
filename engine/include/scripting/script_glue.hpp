#ifndef ENGINEY_CSHARP_GLUE_HPP
#define ENGINEY_CSHARP_GLUE_HPP

#include <iostream>
#include <string>
#include <functional>

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <glm/glm.hpp>

namespace EngineY {

#define ENGINEY_ADD_SCRIPT_FUNCTION(call) mono_add_internal_call("YE.Engine::"#call , (void*)ScriptInternalCalls::call)

    class Entity;

    struct FunctionMaps {
        std::unordered_map<MonoType* , std::function<void(Entity*)>> component_builders{};
        std::unordered_map<MonoType* , std::function<bool(Entity*)>> component_checkers{};
        std::unordered_map<MonoType* , std::function<void(Entity*)>> component_destroyers{};

        template<typename T>
        static std::unordered_map<MonoType* , std::function<bool(Entity* , T*)>> component_getters;

        template<typename T>
        static std::unordered_map<MonoType* , std::function<void(Entity* , T*)>> component_setters;
    };

    class ScriptGlue {
        static void BindTypes();
        static void BindMethods();
        public:
            static void BindAssembly();
            static void UnbindAssembly();
    };

namespace components {

    struct Transform;

}

    class Camera;
    class Scene;

namespace ScriptInternalCalls {

    /// \section Tag Functions
    MonoString* EntityNameFromId(uint64_t entity_id);
    void SetEntityName(uint64_t entity_id , MonoString* name);
    ///////////////////////////

    /// \section Grouping Functions
    ///////////////////////////////

    /// \section Transform Functions
    void GetEntityTransform(uint64_t entity_id , components::Transform* transform);
    void SetEntityTransform(uint64_t entity_id , components::Transform* transform);
    void GetEntityPosition(uint64_t entity_id , glm::vec3* position);
    void SetEntityPosition(uint64_t entity_id , glm::vec3* position);
    void GetEntityScale(uint64_t entity_id , glm::vec3* scale);
    void SetEntityScale(uint64_t entity_id , glm::vec3* scale);
    void GetEntityRotation(uint64_t entity_id , glm::vec3* rotation);
    void SetEntityRotation(uint64_t entity_id , glm::vec3* rotation);
    ////////////////////////////////

    /// \section Renderable Functions
    /////////////////////////////////

    /// \section TexturedRenderable Functions
    /////////////////////////////////////////

    /// \section CubeMapRenderable Functions
    ////////////////////////////////////////

    /// \section RenderableModel Functions
    //////////////////////////////////////

    /// \section DirectionalLight Functions
    ///////////////////////////////////////

    /// \section PointLight Functions
    /////////////////////////////////

    /// \section SpotLight Functions
    ////////////////////////////////

    /// \section PhysicsBody Functions
    uint32_t GetPhysicsBodyType(uint64_t entity_id);
    void SetPhysicsBodyType(uint64_t entity_id , uint32_t type);
    void GetPhysicsBodyPosition(uint64_t entity_id , glm::vec3* position);
    void SetPhysicsBodyPosition(uint64_t entity_id , glm::vec3* position);
    void GetPhysicsBodyRotation(uint64_t entity_id , glm::vec3* rotation);
    void SetPhysicsBodyRotation(uint64_t entity_id , glm::vec3* rotation);
    float GetPhysicsBodyMass(uint64_t entity_id);
    void SetPhysicsBodyMass(uint64_t entity_id , float mass);
    void ApplyForceCenterOfMass(uint64_t entity_id , glm::vec3* force);
    void ApplyForce(uint64_t entity_id , glm::vec3* force , glm::vec3* point);
    void ApplyTorque(uint64_t entity_id , glm::vec3* torque);
    /////////////////////////////////

    /// \section Math Functions
    void Mat4LookAt(glm::vec3* pos , glm::vec3* front , glm::vec3* up , glm::mat4* dest);
    //////////////////////////
    
    /// \section Camera Functions
    uint32_t GetCameraType(uint32_t camera_id);        
    void SetCameraType(uint32_t camera_id, uint32_t type);
    // camera-scene functions
    void RecalculateCameraViewProjectionMatrix(uint32_t camera_id);
    void GetCameraView(uint32_t camera_id , glm::mat4* matrix);
    void GetCameraProjection(uint32_t camera_id , glm::mat4* matrix);
    void GetCameraViewProjection(uint32_t camera_id , glm::mat4* matrix);
    void GetCameraPosition(uint32_t camera_id , glm::vec3* position);
    void SetCameraPosition(uint32_t camera_id , glm::vec3* position);
    void GetCameraFront(uint32_t camera_id , glm::vec3* front);
    void SetCameraFront(uint32_t camera_id , glm::vec3* front);
    void GetCameraUp(uint32_t camera_id , glm::vec3* up);
    void SetCameraUp(uint32_t camera_id , glm::vec3* up);
    void GetCameraRight(uint32_t camera_id , glm::vec3* right);
    void GetCameraWorldUp(uint32_t camera_id , glm::vec3* world_up);
    void SetCameraWorldUp(uint32_t camera_id , glm::vec3* world_up);
    void GetCameraEulerAngles(uint32_t camera_id , glm::vec3* angles);
    void SetCameraEulerAngles(uint32_t camera_id , glm::vec3* angles);
    void GetCameraViewportSize(uint32_t camera_id , glm::vec2* size);
    void SetCameraViewportSize(uint32_t camera_id , glm::vec2* size);
    void GetCameraClip(uint32_t camera_id , glm::vec2* clip);
    void SetCameraClip(uint32_t camera_id , glm::vec2* clip);
    void SetCameraNearClip(uint32_t camera_id , float near_clip);
    void SetCameraFarClip(uint32_t camera_id , float far_clip);
    void GetCameraMousePos(uint32_t camera_id , glm::vec2* position);
    void GetCameraLastMousePos(uint32_t camera_id , glm::vec2* position);
    void GetCameraDeltaMouse(uint32_t camera_id , glm::vec2* delta);
    float GetCameraSpeed(uint32_t camera_id);
    void SetCameraSpeed(uint32_t camera_id, float speed);
    float GetCameraSensitivity(uint32_t camera_id);
    void SetCameraSensitivity(uint32_t camera_id , float sensitivity);
    float GetCameraFov(uint32_t camera_id);
    void SetCameraFov(uint32_t camera_id , float fov);
    float GetCameraZoom(uint32_t camera_id);
    void SetCameraZoom(uint32_t camera_id , float zoom);
    bool GetCameraConstrainPitch(uint32_t camera_id);
    void SetCameraConstrainPitch(uint32_t camera_id , bool constrain_pitch);
    bool IsCameraPrimary(uint32_t camera_id);
    void SetCameraPrimary(uint32_t camera_id , bool primary);
    ///////////////////////////////////

    /// \section Entity Functions
    void EntityAddComponent(uint64_t entity_id , MonoReflectionType* component_name);
    bool EntityHasComponent(uint64_t entity_id , MonoReflectionType* component_name);
    bool EntityRemoveComponent(uint64_t entity_id , MonoReflectionType* component_name);
    uint64_t GetEntityParent(uint64_t entity_id);
    void SetEntityParent(uint64_t child , uint64_t parent);
    /////////////////////////////

    /// \section Scene Functions
    uint64_t CreateEntity(MonoString* name);
    void DestroyEntity(uint64_t entity_id);
    bool IsEntityValid(uint64_t entity);
    uint64_t GetEntityByName(MonoString* name);
    ///////////////////////////

    /// \section Keyboard Functions
    uint32_t KeyFramesHeld(uint32_t key);
    bool IsKeyPressed(uint32_t key);
    bool IsKeyBlocked(uint32_t key);
    bool IsKeyHeld(uint32_t key);
    bool IsKeyDown(uint32_t key);
    bool IsKeyReleased(uint32_t key);
    //////////////////////////////

    /// \section Mouse Functions
    void SnapMouseToCenter();
    void FreeMouse();
    void HideMouse();
    uint32_t MouseX();
    uint32_t MouseY();
    uint32_t MouseLastX();
    uint32_t MouseLastY();
    uint32_t MouseDX();
    uint32_t MouseDY();
    void MousePos(glm::vec2* pos);
    void MouseLastPos(glm::vec2* pos);
    void MouseDelta(glm::vec2* delta);
    bool MouseInWindow();
    uint32_t ButtonFramesHeld(uint32_t button);
    bool IsMouseButtonPressed(uint32_t button);
    bool IsMouseButtonBlocked(uint32_t button);
    bool IsMouseButtonHeld(uint32_t button);
    bool IsMouseButtonReleased(uint32_t button);
    ////////////////////////////

    /// \section Logging Functions
    void LogTrace(MonoString* msg);
    void LogDebug(MonoString* msg);
    void LogInfo(MonoString* msg);
    void LogWarn(MonoString* msg);
    void LogError(MonoString* msg);
    /////////////////////////////

}

}

#endif // !YE_CSHARP_GLUE_HPP