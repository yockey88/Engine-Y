#ifndef YE_SCRIPT_GARBAGE_COLLECTOR_HPP
#define YE_SCRIPT_GARBAGE_COLLECTOR_HPP

#include <mono/metadata/object.h>

namespace YE {
    
    typedef void* GCHandle;
    // typedef uint32_t GCHandle;

    class ScriptGC {
        public:
            static void Initialize();
            static void Shutdown();

            static void Collect(bool blocking = false);

            static GCHandle NewHandle(MonoObject* object , bool weak_ref , bool pinned = false , bool track = true);
            static bool IsValidHandle(GCHandle handle);
            static MonoObject* GetHandleObject(GCHandle handle);
            static void FreeHandle(GCHandle handle);
    };

}

#endif