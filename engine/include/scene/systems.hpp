#ifndef YE_SYSTEMS_HPP
#define YE_SYSTEMS_HPP

#include <entt/entt.hpp>

namespace YE {

namespace components {

    struct Transform;
    struct PhysicsBody;
    struct Renderable;
    struct TexturedRenderable;
    struct RenderableModel;
    struct TextComponent;
    struct PointLight;

}

    class Shader;
    class Entity;
    class Scene;

    using EntityConstructSignal = entt::sigh<void(entt::registry& registry , entt::entity entity)>;
    using EntityCreatedSignal = entt::sigh<void(Scene* context , const std::string&)>;
    using EntityCreatedSink = entt::sink<EntityCreatedSignal>;

    using EntityDestroyedSignal = entt::sigh<void(Scene* context , Entity* entity)>;
    using EntityDestroyedSink = entt::sink<EntityDestroyedSignal>;
    
    using SceneLoadSignal = entt::sigh<void(Scene* context)>;
    using SceneLoadSink = entt::sink<SceneLoadSignal>;

    using SceneUnloadSignal = entt::sigh<void(Scene* context)>;
    using SceneUnloadSink = entt::sink<SceneUnloadSignal>;

    using SceneStartSignal = entt::sigh<void(Scene* context)>;
    using SceneStartSink = entt::sink<SceneStartSignal>;

    using SceneStopSignal = entt::sigh<void(Scene* context)>;
    using SceneStopSink = entt::sink<SceneStopSignal>;

    using UpdateSignal = entt::sigh<void(Scene* scene , float dt)>;
    using UpdateSink = entt::sink<UpdateSignal>;

    using EntityUpdateSignal = entt::sigh<void(components::Transform& transform)>;
    using EntityUpdateSink = entt::sink<EntityUpdateSignal>;

    using PhysicsBodyUpdateSignal = entt::sigh<void(components::PhysicsBody& body , components::Transform& transform)>;
    using PhysicsBodyUpdateSink = entt::sink<PhysicsBodyUpdateSignal>;

    using RenderableUpdateSignal = entt::sigh<void(components::Renderable& renderable , const std::vector<components::PointLight>& lights)>;
    using RenderableUpdateSink = entt::sink<RenderableUpdateSignal>;

    using TexturedRenderableUpdateSignal = entt::sigh<void(components::TexturedRenderable& renderable , const std::vector<components::PointLight>& lights)>;
    using TexturedRenderableUpdateSink = entt::sink<TexturedRenderableUpdateSignal>;

    using RenderableModelUpdateSignal = entt::sigh<void(components::RenderableModel& renderable , const std::vector<components::PointLight>& lights)>;
    using RenderableModelUpdateSink = entt::sink<RenderableModelUpdateSignal>;

    using TextComponentUpdateSignal = entt::sigh<void(components::TextComponent& text , const std::vector<components::PointLight>& lights)>;
    using TextComponentUpdateSink = entt::sink<TextComponentUpdateSignal>;

    class Systems {
        static void LoadShader(Shader *& shader , const std::string& entity_name , const std::string& shader_name , bool& corrupted);

        public:
            static EntityCreatedSignal entity_created_signal;
            static EntityDestroyedSignal entity_destroyed_signal;
            static SceneLoadSignal scene_load_signal;
            static SceneUnloadSignal scene_unload_signal;
            static SceneStartSignal scene_start_signal;
            static SceneStopSignal scene_stop_signal;
            static UpdateSignal update_signal;
            static EntityUpdateSignal entity_update_transform_signal;
            static PhysicsBodyUpdateSignal physics_body_update_signal;
            static RenderableUpdateSignal renderable_update_signal;
            static TexturedRenderableUpdateSignal textured_renderable_update_signal;
            static RenderableModelUpdateSignal renderable_model_update_signal;
            static TextComponentUpdateSignal text_component_update_signal;

            static void Initialize();

            static void SetSceneContext(Scene* context);

            static void EntityConstructed(entt::registry& registry , entt::entity entity);
            static void EntityCreated(Scene* context , const std::string& name);

            static void ModelCreated(entt::registry& registry , entt::entity entity);
            static void PhysicsBodyCreated(entt::registry& registry , entt::entity entity);
            static void BoxColliderCreated(entt::registry& registry , entt::entity entity);
            static void SphereColliderCreated(entt::registry& registry , entt::entity entity);
            static void CapsuleColliderCreated(entt::registry& registry , entt::entity entity);
            static void MeshColliderCreated(entt::registry& registry , entt::entity entity);

            static void LoadShaders(Scene* context);

            static void BindScripts(Scene* context);
            
            static void UpdateScene(Scene* context , float dt);

            static void UpdateTransform(components::Transform& transform);
            static void UpdatePhysicsBody(components::PhysicsBody& body , components::Transform& transform);
            static void UpdateRenderable(components::Renderable& renderable , const std::vector<components::PointLight>& lights);
            static void UpdateTexturedRenderable(components::TexturedRenderable& renderable , const std::vector<components::PointLight>& lights);
            static void UpdateRenderableModel(components::RenderableModel& renderable , const std::vector<components::PointLight>& lights);
            static void UpdateTextComponent(components::TextComponent& text , const std::vector<components::PointLight>& lights);

            static void UnbindScripts(Scene* context);

            static void EntityDestroyed(Scene* context , Entity* entity);
            static void ModelDestroyed(entt::registry& registry , entt::entity entity);
            static void PhysicsBodyDestroyed(entt::registry& context , entt::entity entity);
            static void BoxColliderDestroyed(entt::registry& context , entt::entity entity);
            static void SphereColliderDestroyed(entt::registry& context , entt::entity entity);
            static void CapsuleColliderDestroyed(entt::registry& context , entt::entity entity);
            static void MeshColliderDestroyed(entt::registry& context , entt::entity entity);
        
            static void SceneUnload(Scene* context);

            static void ScenePlay(Scene* context);
            static void StopScene(Scene* context);

            static void CleanupContext(Scene* context);

            static void Teardown();
        private:
            static EntityCreatedSink create_entity_sink;
            static EntityDestroyedSink destroy_entity_sink;
            static SceneLoadSink scene_load_sink;
            static SceneUnloadSink scene_unload_sink;
            static SceneStartSink scene_start_sink;
            static SceneStopSink scene_stop_sink;
            static UpdateSink update_sink;
            static EntityUpdateSink update_entity_sink;
            static PhysicsBodyUpdateSink physics_body_update_sink;
            static RenderableUpdateSink renderable_update_sink;
            static TexturedRenderableUpdateSink textured_renderable_update_sink;
            static RenderableModelUpdateSink renderable_model_update_sink;
            static TextComponentUpdateSink text_component_update_sink;
    };

}

#endif