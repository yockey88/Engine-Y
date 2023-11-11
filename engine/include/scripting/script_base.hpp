#ifndef YE_SCRIPT_UTILS_HPP
#define YE_SCRIPT_UTILS_HPP

#include <vector>
#include <string>

#include <mono/metadata/metadata.h>

#include "log.hpp"
#include "core/UUID.hpp"
#include "garbage_collector.hpp"

#define CHECK_MONO_EXCEPTION(exc)                                                                        \
    if (exc != nullptr) {                                                                                \
        ENGINE_ERROR("Exception thrown by mono [{0} | {1}::{2}]" , __FILE__ , __FUNCTION__ , __LINE__);      \
        YE_CRITICAL_ASSERTION(                                                                           \
            false , "Exception message: {0}" , mono_string_to_utf8(mono_object_to_string(exc , nullptr)) \
        );                                                                                               \
    }

#define CHECK_MONO_ERROR()                                                                      \
    MonoError error;                                                                            \
    bool error = !mono_error_ok(&error);                                                        \
    if (error) {                                                                                \
        ENGINE_ERROR("Error thrown by mono [{0} | {1}::{2}]" , __FILE__ , __FUNCTION__ , __LINE__); \
        unsigned int error_code = mono_error_get_error_code(&error);                            \
        const char* error_name = mono_error_get_error_name(&error);                             \
        ENGINE_ERROR("Error code: {0} | Error name: {1}" , error_code , error_name);                \
        YE_CRITICAL_ASSERTION(                                                                  \
            false , "Error message: {0}" , mono_error_get_message(error)                        \
        );                                                                                      \
    }

// seperating these so we can clearly see what we have a handle on in the code
typedef void* FlagHandle;
typedef void* ParamHandle;
typedef void* MethodHandle;
typedef void* FieldHandle;
typedef void* PropertyHandle;

namespace YE {

    enum class FieldAccess {
        NONE = -1 ,
        READONLY = ybit(0) ,
        STATIC = ybit(1) ,
        PUBLIC = ybit(2) ,
        PRIVATE = ybit(3) ,
        PROTECTED = ybit(4) ,
        INTERNAL = ybit(5) ,
        ARRAY = ybit(6) ,
    };

    enum class FieldType {
        Void = 0 , // cant follow naming convention bc f****** windows !!!!!
        FLOAT , DOUBLE ,
        STRING ,
        BOOL , CHAR , BYTE , SHORT , INT , LONG ,
        UBYTE , USHORT , UINT , ULONG ,
        VECTOR2 , VECTOR3 , VECTOR4 ,
        QUATERNION , MAT4 ,
        ASSET , PREFAB , ENTITY ,
        MESH , STATICMESH , MATERIAL , PHYSICSMATERIAL ,
        TEXTURE 
    };

    struct ScriptField {
        FieldType type = FieldType::Void;
        FieldHandle value = nullptr; // void* to the value
        UUID32 IDU32 = 0;
        uint32_t size = 0;

        uint64_t flags = 0;
        std::string name = "";

        bool is_property = false;

        inline bool HasFlag(FieldAccess flag) const { return flags & (uint64_t)flag; }
        inline bool IsWritable() const { return (!HasFlag(FieldAccess::READONLY) && HasFlag(FieldAccess::PUBLIC)); }
        inline bool IsArray() const { return HasFlag(FieldAccess::ARRAY); }
    };

    struct ScriptMethod {
        MonoMethod* method = nullptr;
        UUID32 IDU32{ 0 };
        uint32_t flags = 0;
        uint32_t param_count = 0;
        std::string name = "";

        bool is_static = false;
        bool virtualm = false;

        ~ScriptMethod() { method = nullptr; }
    };

    struct ScriptObject {
        MonoClass* klass = nullptr;
        UUID32 IDU32{ 0 };
        uint32_t size = 0;
        std::string name;

        std::vector<UUID32> methods{};
        std::vector<UUID32> fields{};

        bool is_custom = false;
        bool abstract = false;
        bool is_struct = false;

        uint32_t parent = 0;

        ~ScriptObject() { klass = nullptr; }
    };

    struct FieldBase {};

    struct Field : public FieldBase {
        FieldHandle handle = nullptr;
        uint64_t size = 0;

        Field() : handle(nullptr) , size(0) {}
        Field(FieldHandle handle , uint64_t size)
            : handle(handle) , size(size) {}
    };

    struct ArrayField : public FieldBase {
        FieldHandle handle = nullptr;
        uint64_t size = 0;
        uint64_t length = 0;
    };

namespace ScriptUtils {

    bool CheckMonoError(MonoError* error);
    std::string MonoStringToStr(MonoString* str);
    FieldType StringToFieldType(const std::string& typestr);
    FieldType FieldTypeFromMonoType(MonoType* mtype);
    std::string FieldTypeToString(FieldType type);
    uint32_t GetTypeSize(FieldType type);
    bool IsPrimitive(FieldType type);
    bool IsAsset(FieldType type);
    uint8_t GetFieldAccess(MonoClassField* field);
    uint8_t GetPropertyAccessibity(MonoProperty* property);
    bool ContainsAttribute(void* attr_lsit , const std::string& name);
    std::string ResolveMonoClassName(MonoClass* klass);

    template<typename Type>
    Type Unbox(MonoObject* obj) { return *(Type*)mono_object_unbox(obj); }

    template<typename Type>
    Type UnboxPointer(MonoObject* obj) { return (Type*)mono_object_unbox(obj); }

}

}

#endif // !YE_SCRIPT_UTILS_HPP