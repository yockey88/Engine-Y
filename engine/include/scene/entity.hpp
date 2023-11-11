#ifndef YE_ENTITY_HPP
#define YE_ENTITY_HPP

#include <entt/entt.hpp>

#include "log.hpp"
#include "scene/scene.hpp"

namespace YE {

    class Entity {
        /// \todo change temporary context to a real context 
        Scene* context;
        entt::entity entity;

        public:
            Entity() 
                : entity(entt::null) , context(nullptr) {}
            Entity(Scene* scene)
                : entity(scene->registry.create()) , context(scene) {}
            ~Entity() {}
            
            void SetParent(Entity* parent);
            void RemoveParent(Entity* parent);

            void GiveChild(Entity* child);
            void RemoveChild(Entity* child);

            Entity* GetParent();

            template<typename T , typename... Args>
            T& AddComponent(Args&&... args) {
                if (HasComponent<T>()) {
                    ENGINE_WARN("Attempted to add a component that already exists");
                    return GetComponent<T>();
                }
                return context->registry.emplace<T>(entity , std::forward<Args>(args)...);
            }

            template<typename T , typename... Args>
            T& AddOrReplace(Args&&... args) {
                return context->registry.emplace_or_replace<T>(entity , std::forward<Args>(args)...);
            }

            template <typename T , typename... Args>
            T& ReplaceComponent(Args&&... args) {
                return context->registry.replace<T>(entity , std::forward<Args>(args)...);
            }

            template<typename T>
            inline T& GetComponent() { return context->registry.get<T>(entity); }

            template<typename T>
            inline bool HasComponent() { return (context->registry.try_get<T>(entity) != nullptr); }

            template<typename T>
            inline void RemoveComponent() { context->registry.remove<T>(entity); }

            inline entt::entity GetEntity() const { return entity; }
            inline bool IsNotNull() const { return entity != entt::null; }
            inline bool IsValid() const { return context->registry.valid(entity); }
            inline bool IsOrphan() const { return context->registry.orphan(entity); }
            inline void SetContext(Scene* scene) { context = scene; }
            inline operator bool() const { return IsNotNull() && IsValid() && !IsOrphan() && (context != nullptr); }
            inline operator entt::entity() const { return entity; }
            inline bool operator==(const Entity& other) const { return entity == other.entity; }
            inline bool operator!=(const Entity& other) const { return entity != other.entity; }
    };
    
}

#endif // !YE_ENTITY_HPP