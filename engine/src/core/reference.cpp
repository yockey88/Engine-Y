#include "core/reference.hpp"

#include "core/log.hpp"

namespace EngineY {

namespace RefUtils {

    static std::unordered_set<void*> living_refs;

    void RegisterReference(void* ref) {
        ENGINE_ASSERT(ref != nullptr , "Reference is nullptr");
        living_refs.insert(ref);
    }

    void RemoveReference(void* ref) {
        ENGINE_ASSERT(ref != nullptr , "Reference is nullptr");
        ENGINE_ASSERT(living_refs.find(ref) != living_refs.end() , "Reference not found");
        living_refs.erase(ref);
    }

    bool IsReferenceValid(void* ref) {
        ENGINE_ASSERT(ref != nullptr , "Reference is nullptr");
        return living_refs.find(ref) != living_refs.end();
    }

}

}
