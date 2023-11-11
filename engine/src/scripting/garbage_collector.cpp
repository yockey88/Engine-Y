#include "scripting/garbage_collector.hpp"

#include <unordered_map>

#include <mono/metadata/object.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/profiler.h>

#include "log.hpp"
#include "core/defines.hpp"

namespace YE {

    typedef std::unordered_map<GCHandle , MonoObject*> RefMap;

    struct GCState {
        RefMap strong_refs;
        RefMap weak_refs;
    };

    static GCState* gc_state = nullptr;

    void ScriptGC::Initialize() {
        YE_CRITICAL_ASSERTION(gc_state == nullptr , "ScriptGC already initialized");
        gc_state = ynew GCState();
    }
    
    void ScriptGC::Shutdown() {

        if (gc_state->strong_refs.size() > 0) {
            ENGINE_ERROR("ScriptGC: {0} strong references not freed , memory leak detected" , gc_state->strong_refs.size());

            for (auto[handle , object] : gc_state->strong_refs) 
                mono_gchandle_free_v2(handle);

            gc_state->strong_refs.clear();
        }

        if (gc_state->weak_refs.size() > 0) {
            ENGINE_ERROR("ScriptGC: {0} weak references not freed , memory leak detected" , gc_state->weak_refs.size());

            for (auto[handle , object] : gc_state->weak_refs) 
                mono_gchandle_free_v2(handle);

            gc_state->weak_refs.clear();
        }

        mono_gc_collect(mono_gc_max_generation());
        while (mono_gc_pending_finalizers() > 0);

        ydelete gc_state;
        gc_state = nullptr;
    }

    void ScriptGC::Collect(bool blocking) {
        mono_gc_collect(mono_gc_max_generation());
        if (blocking) {
            while (mono_gc_pending_finalizers() > 0);
        }
    }

    GCHandle ScriptGC::NewHandle(MonoObject* object , bool weak_ref , bool pinned , bool track) {
        GCHandle handle = weak_ref ?
            mono_gchandle_new_weakref_v2(object , weak_ref) :
            mono_gchandle_new_v2(object , weak_ref);

        YE_CRITICAL_ASSERTION(handle != 0 , "Failed to create GCHandle");

        if (track) {
            if (weak_ref) {
                YE_CRITICAL_ASSERTION(gc_state->weak_refs.find(handle) == gc_state->weak_refs.end() , "GCHandle already tracked");
                gc_state->weak_refs[handle] = object;
            } else {
                YE_CRITICAL_ASSERTION(gc_state->strong_refs.find(handle) == gc_state->strong_refs.end() , "GCHandle already tracked");
                gc_state->strong_refs[handle] = object;
            }
        }

        return handle;
    }

    bool ScriptGC::IsValidHandle(GCHandle handle) {
        if (handle == 0)
            return false;

        MonoObject* object = mono_gchandle_get_target_v2(handle);

        if (object == nullptr)
            return false;

        if (mono_object_get_vtable(object) == nullptr)
            return false;

        return true;
    }

    MonoObject* ScriptGC::GetHandleObject(GCHandle handle) {
        MonoObject* object = mono_gchandle_get_target_v2(handle);
        if (object == nullptr || mono_object_get_vtable(object) == nullptr)
            return nullptr;
        return object;
    }

    void ScriptGC::FreeHandle(GCHandle handle) {
        YE_CRITICAL_ASSERTION(handle != nullptr , "GCHandle is null");

        // uint32_t ghandle = mono_object_get_(handle);
        if (mono_gchandle_get_target_v2(handle) != nullptr) {
            mono_gchandle_free_v2(handle);
        } else {
            ENGINE_ERROR("Tried to release invalid GCHandle");
            return;
        }

        if (gc_state->strong_refs.find(handle) != gc_state->strong_refs.end()) {
            gc_state->strong_refs.erase(handle);
        }
        if (gc_state->weak_refs.find(handle) != gc_state->weak_refs.end())
            gc_state->weak_refs.erase(handle);
    }

}