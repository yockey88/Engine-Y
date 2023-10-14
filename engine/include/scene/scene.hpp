#ifndef YE_SCENE_HPP
#define YE_SCENE_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <entt/entt.hpp>

#include "log.hpp"
#include "systems.hpp"
#include "core/RNG.hpp"
#include "core/UUID.hpp"
#include "rendering/renderer.hpp"

constexpr uint32_t kMaxPointLights = 128;

namespace YE {

namespace components {

    struct ID;
    struct Grouping;
    struct Transform;
    struct Movable;
    struct Renderable;
    struct TexturedRenderable;
    struct NativeScript;
    struct Script;
}

    class Entity;
    class VertexArray;
    class Shader;
    class Texture;
    class Camera;

    template<typename T>
    using SceneMapU64 = typename std::unordered_map<UUID , T*>;

    template<typename T>
    using SceneMapU32 = std::unordered_map<UUID32 , T*>;

    class Scene {
        RNG::RngEngineU64 rng;
        UUID sceneID = 0;

        entt::entity scene_entity = entt::null;
        entt::registry registry;

        std::vector<UUID> living_entities;

        Camera* active_camera = nullptr;
        UUID32 active_camera_id = 0;
        RenderMode current_render_mode = RenderMode::FILL;
        
        SceneMapU64<Entity> entities;
        SceneMapU64<Shader> shaders;
        SceneMapU32<Camera> cameras;

        std::string scene_name = "[Blank Scene]";
        UUID scene_id = 0;

        bool render_debug = false;

        friend class Entity;
        friend class Systems;
        friend class Renderer;

        public:
            Scene(const std::string& name)
                : scene_name(name) , scene_id(Hash::FNV(name)) {}
            ~Scene() {}

            Entity* CreateEntity(const std::string& name = "[Blank Entity]");
            Entity* GetEntity(UUID id);
            Entity* GetEntity(const std::string& name);

            void DestroyEntity(Entity* entity);
            // void DestroyEntity(UUID id);
            // void DestroyEntity(const std::string& name);

            bool IsEntityValid(UUID id);

            std::unordered_map<UUID , Entity*>* Entities();

            void InitializeScene();
            void LoadScene(/* not sure what would be passed here */);
            void UnloadScene(/* not sure what would be passed here */);

            Camera* AttachCamera(const std::string& name = "[Blank Camera]" , CameraType type = CameraType::PERSPECTIVE);
            Camera* GetCamera(const std::string& name);
            Camera* GetCamera(UUID32 id);
            void SetActiveCamera(UUID32 id);
            void SetActiveCamera(const std::string& name);
            
            void SetRenderMode(RenderMode mode);

            void Start();
            void Update(float dt);
            void Draw();
            void End();
            void Shutdown();

            inline std::string SceneName() const { return scene_name; }
            inline entt::registry& Registry() { return registry; }
            inline Camera* ActiveCamera() { return active_camera; }
            inline UUID SceneID() const { return sceneID; }
            inline void ActivateDebug() { render_debug = true; }
            inline void DeactivateDebug() { render_debug = false; }

    };

};

#endif // !YE_SCENE_HPP