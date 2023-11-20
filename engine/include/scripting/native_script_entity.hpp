#ifndef ENGINEY_NATIVE_SCRIPT_ENTITY_HPP
#define ENGINEY_NATIVE_SCRIPT_ENTITY_HPP

#include "entt/entt.hpp"

#include "scene/entity.hpp"

namespace EngineY {

    class NativeScriptEntity {
        public:
            Entity entity;

            NativeScriptEntity() {}
            virtual ~NativeScriptEntity() {}

            Entity& Get() { return entity; }
            Entity& operator->() { return entity; }

            virtual void Create() = 0;
            virtual void Update(float dt) = 0;
            virtual void Destroy() = 0;
    };

}

#endif // !YE_NATIVE_SCRIPT_ENTITY_HPP
