#include "scripting/script_base.hpp"

#include "mono/metadata/attrdefs.h"

namespace YE {
    

namespace ScriptUtils {

    bool CheckMonoError(MonoError* error) {
        bool has_error = !mono_error_ok(error);
        if (has_error) {
            unsigned short err_code = mono_error_get_error_code(error);
            std::string err_msg{ mono_error_get_message(error) };
            ENGINE_ERROR("Mono error: {0} | {1}" , err_code , err_msg);
            mono_error_cleanup(error);
        }
        return has_error;
    }

    std::string MonoStringToStr(MonoString* str) {
        if (str == nullptr || mono_string_length(str) == 0) return "";

        MonoError error{};
        char* utf8 = mono_string_to_utf8_checked(str , &error);
        if (CheckMonoError(&error)) return "";

        std::string result{ utf8 };
        mono_free(utf8);

        return result;
    }

    FieldType StringToFieldType(const std::string& typestr) {
        if (typestr == "Void") return FieldType::Void;
        if (typestr == "Single") return FieldType::FLOAT;
        if (typestr == "Double") return FieldType::DOUBLE;
        if (typestr == "String") return FieldType::STRING;
        if (typestr == "Boolean") return FieldType::BOOL;
        if (typestr == "Char") return FieldType::CHAR;
        if (typestr == "SByte") return FieldType::BYTE;
        if (typestr == "Int16") return FieldType::SHORT;
        if (typestr == "Int32") return FieldType::INT;
        if (typestr == "Int64") return FieldType::LONG;
        if (typestr == "Byte") return FieldType::UBYTE;
        if (typestr == "UInt16") return FieldType::USHORT;
        if (typestr == "UInt32") return FieldType::UINT;
        if (typestr == "UInt64") return FieldType::ULONG;
        if (typestr == "Vec2") return FieldType::VECTOR2;
        if (typestr == "Vec3") return FieldType::VECTOR3;
        if (typestr == "Vec4") return FieldType::VECTOR4;
        if (typestr == "Entity") return FieldType::ENTITY;
        
        /// \todo implement the rest of the components in ScriptUtils::StringToType
        // if (typestr == "Quaternion") return FieldType::QUATERNION;
        // if (typestr == "Matrix4x4") return FieldType::MATRIX4X4;
        // if (typestr == "Asset") return FieldType::ASSET;
        // if (typestr == "Prefab") return FieldType::PREFAB;
        // if (typestr == "Mesh") return FieldType::MESH;
        // if (typestr == "StaticMesh") return FieldType::STATICMESH;
        // if (typestr == "Material") return FieldType::MATERIAL;
        // if (typestr == "PhysicsMaterial") return FieldType::PHYSICSMATERIAL;
        // if (typestr == "Texture") return FieldType::TEXTURE;

        return FieldType::Void;
    }

    FieldType FieldTypeFromMonoType(MonoType* mtype) {
        char* name = mono_type_get_name(mtype);
        std::string typestr{ name };
        typestr = typestr.substr(typestr.find('.') + 1 , typestr.length() - 1);
        return StringToFieldType(typestr);
    }

    std::string FieldTypeToString(FieldType type) {
        switch (type) {
            case FieldType::Void: return "Void";
            case FieldType::FLOAT: return "Float";
            case FieldType::DOUBLE: return "Double";
            case FieldType::STRING: return "String";
            case FieldType::BOOL: return "Bool";
            case FieldType::CHAR: return "Char";
            case FieldType::BYTE: return "Byte";
            case FieldType::SHORT: return "Int16";
            case FieldType::INT: return "Int32";
            case FieldType::LONG: return "Int64";
            case FieldType::UBYTE: return "UByte";
            case FieldType::USHORT: return "UInt16";
            case FieldType::UINT: return "UInt32";
            case FieldType::ULONG: return "UInt64";
            case FieldType::VECTOR2: return "Vec2";
            case FieldType::VECTOR3: return "Vec3";
            case FieldType::VECTOR4: return "Vec4";
            case FieldType::ENTITY: return "Entity";

            /// \todo implement rest of the components in ScriptUtils::TypeToString
            // case FieldType::QUATERNION: return "Quaternion";
            // case FieldType::MATRIX4X4: return "Matrix4x4";
            // case FieldType::ASSET: return "Asset";
            // case FieldType::PREFAB: return "Prefab";
            // case FieldType::MESH: return "Mesh";
            // case FieldType::STATICMESH: return "StaticMesh";
            // case FieldType::MATERIAL: return "Material";
            // case FieldType::PHYSICSMATERIAL: return "PhysicsMaterial";
            // case FieldType::TEXTURE: return "Texture";
            default: break;
        }

        YE_CRITICAL_ASSERTION(false , "Unknown script type encountered!");
        return "Unknown";
    }
    
    uint32_t GetTypeSize(FieldType type) {
        switch (type) {
            case FieldType::FLOAT: return sizeof(float);
            case FieldType::DOUBLE: return sizeof(double);
            case FieldType::BOOL: return sizeof(bool);
            case FieldType::CHAR: return sizeof(char);
            case FieldType::BYTE: return sizeof(uint8_t);
            case FieldType::SHORT: return sizeof(int16_t);
            case FieldType::INT: return sizeof(int32_t);
            case FieldType::LONG: return sizeof(int64_t);
            case FieldType::UBYTE: return sizeof(uint8_t);
            case FieldType::USHORT: return sizeof(uint16_t);
            case FieldType::UINT: return sizeof(uint32_t);
            case FieldType::ULONG: return sizeof(uint64_t);
            case FieldType::VECTOR2: return sizeof(glm::vec2);
            case FieldType::VECTOR3: return sizeof(glm::vec3);
            case FieldType::VECTOR4: return sizeof(glm::vec4);
            case FieldType::QUATERNION: return sizeof(glm::quat);
            case FieldType::MAT4: return sizeof(glm::mat4);
            case FieldType::ASSET:
            case FieldType::PREFAB:
            case FieldType::ENTITY:
            case FieldType::MESH:
            case FieldType::STATICMESH:
            case FieldType::MATERIAL:
            case FieldType::PHYSICSMATERIAL:
            case FieldType::TEXTURE:
            default: break;
        }

        YE_CRITICAL_ASSERTION(false , "Unknown script type encountered!");
        return 0;
    }

    bool IsPrimitive(FieldType type) {
        switch (type) {
            case FieldType::STRING: return false;
            case FieldType::VECTOR2: return false;
            case FieldType::VECTOR3: return false;
            case FieldType::VECTOR4: return false;
            case FieldType::QUATERNION: return false;
            case FieldType::MAT4: return false;
            case FieldType::ASSET: return false;
            case FieldType::PREFAB: return false;
            case FieldType::ENTITY: return false;
            case FieldType::MESH: return false;
            case FieldType::STATICMESH: return false;
            case FieldType::MATERIAL: return false;
            case FieldType::PHYSICSMATERIAL: return false;
            case FieldType::TEXTURE: return false;
            default: return true;
        }

        YE_CRITICAL_ASSERTION(false , "UNREACHABLE CODE!");
        return false;
    }

    bool IsAsset(FieldType type) {
        switch (type) {
            case FieldType::ASSET: return true;
            case FieldType::PREFAB: return true;
            case FieldType::MESH: return true;
            case FieldType::STATICMESH: return true;
            case FieldType::MATERIAL: return true;
            case FieldType::PHYSICSMATERIAL: return true;
            case FieldType::TEXTURE: return true;
            default: return false;
        }

        YE_CRITICAL_ASSERTION(false , "UNREACHABLE CODE!");
        return false;
    }
    
    uint8_t GetFieldAccess(MonoClassField* field) {
        uint8_t access = static_cast<uint8_t>(FieldAccess::NONE);
        uint32_t access_flag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

        switch (access_flag) {
            case MONO_FIELD_ATTR_PRIVATE:
                access &= ~static_cast<uint8_t>(FieldAccess::PROTECTED);
                access &= ~static_cast<uint8_t>(FieldAccess::INTERNAL);
                access &= ~static_cast<uint8_t>(FieldAccess::PUBLIC);
                access |= static_cast<uint8_t>(FieldAccess::PRIVATE);
            break;
            case MONO_FIELD_ATTR_FAM_AND_ASSEM:
                access |= static_cast<uint8_t>(FieldAccess::PROTECTED);
                access |= static_cast<uint8_t>(FieldAccess::INTERNAL);
            break;
            case MONO_FIELD_ATTR_ASSEMBLY:
                access &= ~static_cast<uint8_t>(FieldAccess::PRIVATE);
                access &= ~static_cast<uint8_t>(FieldAccess::PROTECTED);
                access &= ~static_cast<uint8_t>(FieldAccess::PUBLIC);
                access |= static_cast<uint8_t>(FieldAccess::INTERNAL);
            break;
            case MONO_FIELD_ATTR_FAMILY:
                access &= ~static_cast<uint8_t>(FieldAccess::PRIVATE);
                access &= ~static_cast<uint8_t>(FieldAccess::PUBLIC);
                access &= ~static_cast<uint8_t>(FieldAccess::INTERNAL);
                access |= static_cast<uint8_t>(FieldAccess::PROTECTED);
            break;
            case MONO_FIELD_ATTR_FAM_OR_ASSEM: 
                access |= static_cast<uint8_t>(FieldAccess::PRIVATE);
                access |= static_cast<uint8_t>(FieldAccess::PROTECTED);
            break;
            case MONO_FIELD_ATTR_PUBLIC:
                access &= ~static_cast<uint8_t>(FieldAccess::PRIVATE);
                access &= ~static_cast<uint8_t>(FieldAccess::PROTECTED);
                access &= ~static_cast<uint8_t>(FieldAccess::INTERNAL);
                access |= static_cast<uint8_t>(FieldAccess::PUBLIC);
            break;
        }

        return access;
    }

    
    uint8_t GetPropertyAccessibity(MonoProperty* property) {
        uint8_t access = static_cast<uint8_t>(FieldAccess::NONE);

        MonoMethod* getter = mono_property_get_get_method(property);
        if (getter == nullptr) {
            uint32_t maccess = mono_method_get_flags(getter , nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

            switch (access) {
                case MONO_FIELD_ATTR_PRIVATE: access |= static_cast<uint8_t>(FieldAccess::PRIVATE); break;
                case MONO_FIELD_ATTR_FAM_AND_ASSEM:
                    access |= static_cast<uint8_t>(FieldAccess::PROTECTED);
                    access |= static_cast<uint8_t>(FieldAccess::INTERNAL);
                break;
                case MONO_FIELD_ATTR_ASSEMBLY: access |= static_cast<uint8_t>(FieldAccess::INTERNAL); break;
                case MONO_FIELD_ATTR_FAMILY: access |= static_cast<uint8_t>(FieldAccess::PROTECTED); break;
                case MONO_FIELD_ATTR_FAM_OR_ASSEM: 
                    access |= static_cast<uint8_t>(FieldAccess::PRIVATE);
                    access |= static_cast<uint8_t>(FieldAccess::PROTECTED);
                break;
                case MONO_FIELD_ATTR_PUBLIC: access |= static_cast<uint8_t>(FieldAccess::PUBLIC); break;
            }
        }

        MonoMethod* setter = mono_property_get_set_method(property);
        if (setter != nullptr) {
            uint32_t maccess = mono_method_get_flags(setter , nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;
            if (maccess != MONO_FIELD_ATTR_PUBLIC)
                access |= static_cast<uint8_t>(FieldAccess::PRIVATE);
        }  else {
            access |= static_cast<uint8_t>(FieldAccess::PRIVATE);
        }

        return access;
    }
    
    bool ContainsAttribute(void* attr_lsit , const std::string& name) {
        // const ScriptObject* attr_class = ScriptMap::GetObjByName(name);

        // if (attr_class == nullptr)
        //     return false;

        // if (attr_list == nullptr)
        //     return false;
        
        // return mono_custom_attrs_has_attr(attr_list , attr_class->GetClass());
        return false;
    }

    std::string ResolveMonoClassName(MonoClass* klass) {
        const char* namep = mono_class_get_name(klass);
        std::string name = (namep == nullptr) ? 
                                "" : name;
        
        if (name == "")
            return "[Unknown Class]";

        MonoClass* parent = mono_class_get_parent(klass);
        if (parent != nullptr) {
            name = ResolveMonoClassName(parent) + "/" + name;
        } else {
            const char* ns = mono_class_get_namespace(klass);
            if (ns != nullptr)
                name = std::string(ns) + "." + name;
        }

        MonoType* type = mono_class_get_type(klass);
        if (mono_type_get_type(type) == MONO_TYPE_SZARRAY || mono_type_get_type(type) == MONO_TYPE_ARRAY) {
            const char* chars = "[]";
            for (size_t i = 0; i < strlen(chars); i++)
				name.erase(std::remove(name.begin(), name.end(), chars[i]), name.end());
        }

        return name;
    }

}

}