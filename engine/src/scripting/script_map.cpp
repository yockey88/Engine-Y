#include "scripting/script_map.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/tokentype.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>

#include "log.hpp"
#include "core/defines.hpp"
#include "core/UUID.hpp"
#include "scripting/script_engine.hpp"

namespace YE {

    struct ScriptObjectMap {
        std::unordered_map<UUID32 , ScriptObject> script_objs;
        std::unordered_map<UUID32 , ScriptField> script_fields;
        std::unordered_map<UUID32 , std::vector<ScriptMethod>> script_methods;
    };

    static ScriptObjectMap* script_map = nullptr;

    void ScriptMap::StoreCoreClasses() {
        if (script_map == nullptr) return;

#define CACHE_MSCORLIB_CLASS(name) StoreClass("System::" ##name , mono_class_from_name(mono_get_corlib() , "System" , name))

        CACHE_MSCORLIB_CLASS("Object");
        CACHE_MSCORLIB_CLASS("ValueType");
        CACHE_MSCORLIB_CLASS("Enum");
        CACHE_MSCORLIB_CLASS("String");
        CACHE_MSCORLIB_CLASS("Boolean");
        CACHE_MSCORLIB_CLASS("Char");
        CACHE_MSCORLIB_CLASS("SByte");
        CACHE_MSCORLIB_CLASS("Byte");
        CACHE_MSCORLIB_CLASS("Int16");
        CACHE_MSCORLIB_CLASS("UInt16");
        CACHE_MSCORLIB_CLASS("Int32");
        CACHE_MSCORLIB_CLASS("UInt32");
        CACHE_MSCORLIB_CLASS("Int64");
        CACHE_MSCORLIB_CLASS("UInt64");
        CACHE_MSCORLIB_CLASS("Single");
        CACHE_MSCORLIB_CLASS("Double");
        CACHE_MSCORLIB_CLASS("Void");

#undef CACHE_MSCORLIB_CLASS
#define CACHE_YENGINE_CLASS(name) StoreClass(                                                                                         \
                                        "YE." ##name , mono_class_from_name(ScriptEngine::Instance()->internal_script_data.image ,    \
                                                                            "YE" , name))
        
        CACHE_YENGINE_CLASS("Vec2");
        CACHE_YENGINE_CLASS("Vec3");
        CACHE_YENGINE_CLASS("Vec4");
        CACHE_YENGINE_CLASS("Entity");

#undef CACHE_YENGINE_CLASS
    }
        
    void ScriptMap::StoreClass(std::string_view name , MonoClass* klass) {
        MonoType* ctype = mono_class_get_type(klass);
        ScriptObject obj{};
        obj.name = name;
        obj.IDU32 = Hash::FNV32(obj.name);

        uint32_t alignment = 0;
        obj.size = mono_class_value_size(klass , &alignment);
        obj.klass = klass;
        script_map->script_objs[obj.IDU32] = obj;
        if (obj.name.find("YE.") != std::string::npos) {
            StoreClassMethods(obj);
            StoreClassFields(obj);
            StoreClassProperties(obj);
        }
    }
    
    void ScriptMap::StoreClassMethods(ScriptObject& klass) {
        YE_CRITICAL_ASSERTION(klass.klass != nullptr , "Retrieving methods from null class!");
        MonoMethod* method = nullptr;
        MethodHandle iter = nullptr;

        do {
            method = mono_class_get_methods(klass.klass , &iter);
            if (method == nullptr) break;

            MonoMethodSignature* signature = mono_method_signature(method);
            ScriptMethod sm{};

            /// \note checking for duplicate IDs
            sm.name = std::string{ mono_method_full_name(method , 0) }; // 0 for false
            // YE_INFO("Method: {0}" , sm.name);

            std::string hash_str = sm.name;
            UUID32 ID{ Hash::FNV32(hash_str) };
            
            sm.IDU32 = ID;
            
            sm.flags = mono_method_get_flags(method , nullptr);

            sm.is_static = sm.flags & MONO_METHOD_ATTR_STATIC;
            sm.virtualm = sm.flags & MONO_METHOD_ATTR_VIRTUAL;
            sm.param_count = mono_signature_get_param_count(signature);
            sm.method = method;

            script_map->script_methods[sm.IDU32].push_back(sm);
            klass.methods.push_back(sm.IDU32);

        } while (method != nullptr);
    }
    
    void ScriptMap::StoreClassFields(ScriptObject& klass) {
        YE_CRITICAL_ASSERTION(klass.klass != nullptr , "Attempting to retrieve fields from null class");

        std::string name = std::string{ mono_class_get_name(klass.klass) };
        std::string name_space = std::string{ mono_class_get_namespace(klass.klass) };

        MonoClass* curr_class = klass.klass;
        while (curr_class != nullptr) {
            std::string class_name{ mono_class_get_name(curr_class) };
            std::string class_namespace{ mono_class_get_namespace(curr_class) };

            /// \todo since entity is special we might want to handle this seperately
            // if (class_namespace.find("YE") != std::string::npos && class_name.find("Entity") != std::string::npos) {
            //     YE_DEBUG("Found Entity Class");
            //     curr_class = nullptr;
            //     continue;
            // }

            MonoClassField* field = nullptr;
            FieldHandle iter = nullptr;

            do {
                field = mono_class_get_fields(curr_class , &iter);
                if (field == nullptr) 
                    break;

                std::string name{ mono_field_get_name(field) };

                // field included from properties that I don't want (See Hazel Engine by TheCherno)
                if (name.find("k__BackingField") != std::string::npos)
                    continue;

                MonoType* mtype = mono_field_get_type(field);
                FieldType ftype = ScriptUtils::FieldTypeFromMonoType(mtype);

                if (ftype == FieldType::Void)
                    continue;

                MonoCustomAttrInfo* attr = mono_custom_attrs_from_field(curr_class , field);

                std::string hash_str = klass.name + ":" + name;
                UUID32 ID{ Hash::FNV32(hash_str) };

                int32_t type_encoding = mono_type_get_type(mtype);
                
                ScriptField& sf = script_map->script_fields[ID];

                sf.name = std::string{ mono_field_get_name(field) };
                sf.IDU32 = ID;
                sf.type = ftype;
                sf.is_property = false;

                if (type_encoding == MONO_TYPE_ARRAY || type_encoding == MONO_TYPE_SZARRAY) 
                    sf.flags |= static_cast<uint64_t>(FieldAccess::ARRAY);

                uint32_t visibility = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
				switch (visibility) {
					case MONO_FIELD_ATTR_PUBLIC:
						sf.flags &= ~(uint64_t)FieldAccess::PROTECTED;
						sf.flags &= ~(uint64_t)FieldAccess::PRIVATE;
						sf.flags &= ~(uint64_t)FieldAccess::INTERNAL;
						sf.flags |= (uint64_t)FieldAccess::PUBLIC;
                    break;
					case MONO_FIELD_ATTR_FAMILY:
						sf.flags &= ~(uint64_t)FieldAccess::PUBLIC;
						sf.flags &= ~(uint64_t)FieldAccess::PRIVATE;
						sf.flags &= ~(uint64_t)FieldAccess::INTERNAL;
						sf.flags |= (uint64_t)FieldAccess::PROTECTED;
                    break;
					case MONO_FIELD_ATTR_ASSEMBLY:
						sf.flags &= ~(uint64_t)FieldAccess::PUBLIC;
						sf.flags &= ~(uint64_t)FieldAccess::PROTECTED;
						sf.flags &= ~(uint64_t)FieldAccess::PRIVATE;
						sf.flags |= (uint64_t)FieldAccess::INTERNAL;
                    break;
					case MONO_FIELD_ATTR_PRIVATE:
						sf.flags &= ~(uint64_t)FieldAccess::PUBLIC;
						sf.flags &= ~(uint64_t)FieldAccess::PROTECTED;
						sf.flags &= ~(uint64_t)FieldAccess::INTERNAL;
						sf.flags |= (uint64_t)FieldAccess::PRIVATE;
                    break;
				}
                
                int align;
                if (sf.IsArray()) {
                    MonoClass* field_array_class = mono_type_get_class(mtype);
                    MonoClass* elt_class = mono_class_get_element_class(field_array_class);
                    MonoType* elt_type = mono_class_get_type(elt_class);
                    sf.size = mono_type_size(elt_type , &align);
                } else {
                    sf.size = mono_type_size(mtype , &align);
                }

                klass.size += sf.size;

                if (std::find(klass.fields.begin() , klass.fields.end() , sf.IDU32) == klass.fields.end())
                    klass.fields.push_back(sf.IDU32);

            } while (field != nullptr);

            curr_class = mono_class_get_parent(curr_class);

        } 
    }
   
    void ScriptMap::StoreClassProperties(ScriptObject& klass) {
        MonoProperty* prop = nullptr;
        PropertyHandle iter = nullptr;
        do {
            prop = mono_class_get_properties(klass.klass , &iter);
            if (prop == nullptr) break;

            std::string name{ mono_property_get_name(prop) };

            std::string hash_str = klass.name + ":" + name;
            UUID32 prop_id{ Hash::FNV32(hash_str) };

            uint64_t prop_flags = 0;
            MonoType* mtype = nullptr;

            MonoMethod* getter = mono_property_get_get_method(prop);
            if (getter != nullptr) {
                MonoMethodSignature* sig = mono_method_signature(getter);
                mtype = mono_signature_get_return_type(sig);

                uint32_t flags = mono_method_get_flags(getter , nullptr);
                uint32_t visibility = flags & MONO_METHOD_ATTR_ACCESS_MASK;

                switch (visibility) {
					case MONO_FIELD_ATTR_PUBLIC: 
						prop_flags &= ~(uint64_t)FieldAccess::PROTECTED;
						prop_flags &= ~(uint64_t)FieldAccess::PRIVATE;
						prop_flags &= ~(uint64_t)FieldAccess::INTERNAL;
						prop_flags |= (uint64_t)FieldAccess::PUBLIC;
                    break;
					case MONO_FIELD_ATTR_FAMILY:
						prop_flags &= ~(uint64_t)FieldAccess::PUBLIC;
						prop_flags &= ~(uint64_t)FieldAccess::PRIVATE;
						prop_flags &= ~(uint64_t)FieldAccess::INTERNAL;
						prop_flags |= (uint64_t)FieldAccess::PROTECTED;
                    break;
					case MONO_FIELD_ATTR_ASSEMBLY:
						prop_flags &= ~(uint64_t)FieldAccess::PUBLIC;
						prop_flags &= ~(uint64_t)FieldAccess::PROTECTED;
						prop_flags &= ~(uint64_t)FieldAccess::PRIVATE;
						prop_flags |= (uint64_t)FieldAccess::INTERNAL;
                    break;
					case MONO_FIELD_ATTR_PRIVATE:
						prop_flags &= ~(uint64_t)FieldAccess::PUBLIC;
						prop_flags &= ~(uint64_t)FieldAccess::PROTECTED;
						prop_flags &= ~(uint64_t)FieldAccess::INTERNAL;
						prop_flags |= (uint64_t)FieldAccess::PRIVATE;
                    break;
				}

                if ((flags & MONO_METHOD_ATTR_STATIC) != 0)
                    prop_flags |= static_cast<uint64_t>(FieldAccess::STATIC);
            }

            MonoMethod* setter = mono_property_get_set_method(prop);
            if (setter != nullptr) {
                ParamHandle iter = nullptr;
                MonoMethodSignature* sig = mono_method_signature(setter);
                mtype = mono_signature_get_params(sig , &iter);

                uint32_t flags = mono_method_get_flags(setter , nullptr);
                if ((flags & MONO_METHOD_ATTR_ACCESS_MASK) == MONO_METHOD_ATTR_PRIVATE)
                    prop_flags |= static_cast<uint64_t>(FieldAccess::PRIVATE);

                if ((flags & MONO_METHOD_ATTR_STATIC) != 0)
                    prop_flags |= static_cast<uint64_t>(FieldAccess::STATIC);
            }

            if (getter == nullptr && setter == nullptr)
                prop_flags |= static_cast<uint64_t>(FieldAccess::READONLY);

            if (mtype == nullptr) {
                YE_ERROR("Failed to retrieve type for property: {0}" , name);
                continue;
            }

            MonoCustomAttrInfo* attrs = mono_custom_attrs_from_property(klass.klass , prop);

            /// \todo this
            // if (!ScriptUtils::ContainsAttribute(attrs , "NoAttrsImplemented"));
                // continue;
            
            ScriptField& sf = script_map->script_fields[prop_id];
            sf.name = name;
            sf.IDU32 = prop_id;
            sf.type = ScriptUtils::FieldTypeFromMonoType(mtype);
            sf.is_property = true;

            int align;
            sf.size = mono_type_size(mtype , &align);
            klass.size += sf.size;
            if (std::find(klass.fields.begin() , klass.fields.end() , sf.IDU32) == klass.fields.end())
                klass.fields.push_back(sf.IDU32);

        } while (prop != nullptr);
    }

    ScriptObject* ScriptMap::GetClassByName(const std::string& name) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");

        uint32_t id = Hash::FNV32(name);
        return GetClassByID(id);
    }

    ScriptObject* ScriptMap::GetClassByID(UUID32 id) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");

        if (script_map->script_objs.find(id) != script_map->script_objs.end())
            return &script_map->script_objs[id];

        return nullptr;
    }

    ScriptObject* ScriptMap::GetClass(MonoClass* klass) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");

        if (klass == nullptr) 
            return nullptr;
        
        return GetClassByName(ScriptUtils::ResolveMonoClassName(klass));
    }

    ScriptObject* ScriptMap::GetObjectFromClass(MonoObject* obj) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");

        MonoClass* obj_class = mono_object_get_class(obj);
        if (obj_class == nullptr)
            return nullptr;

        return GetClassByName(ScriptUtils::ResolveMonoClassName(obj_class));
    }


    ScriptMethod* ScriptMap::GetMethodByName(ScriptObject* obj , const std::string& name , bool virtualm) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to access method from null object");

        uint32_t id = Hash::FNV32(obj->name + ":" + name);
        if (script_map->script_methods.find(id) != script_map->script_methods.end())
            return &script_map->script_methods[id][0];
        
        if (!virtualm && obj->parent != 0) 
            return GetMethodByName(&script_map->script_objs.at(obj->parent) , name);

        YE_FATAL("Failed to find method: {0} in class: {1}" , name , obj->name);
        return nullptr;
    }

    ScriptMethod* ScriptMap::GetMethod(ScriptObject* obj , const std::string& name , uint32_t num_params , bool virtualm) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to access method from null object");

        ScriptMethod* method = nullptr;
        uint32_t id = Hash::FNV32(obj->name + ":" + name);
        if (script_map->script_methods.find(id) != script_map->script_methods.end()) {
            for (auto& m : script_map->script_methods.at(id)) {
                if (m.param_count == num_params) {
                    method = &m;
                    break;
                }
            }
        }

        if (method == nullptr && !virtualm && obj->parent != 0) 
            method = GetMethod(&script_map->script_objs.at(obj->parent) , name , num_params);

        // if (method == nullptr && virtualm)
        
        if (method == nullptr) {
            YE_FATAL("Failed to find method: {0} in class: {1}" , name , obj->name);
            return nullptr;
        }

        return method;
    }

    // Container ScriptMap::ObjectToValue(MonoObject* obj , ScriptField* field) {
    //     if (object == nullptr)
    //         return Container{};

    //     Container c{};
    //     c.Allocate(ScriptUtils::GetTypeSize(field->type));
    //     c.ZeroOut();

    //     switch (field->type) {
    //         case FieldType::BOOL:
    //             bool val = Unbox<bool>(obj);
    //             c.Write(&val , sizeof(bool));
    //         break;
    //         case FieldType::BYTE:
    //             int8_t i8 = Unbox<int8_t>(obj);
    //             c.Write(&i8 , sizeof(int8_t));
    //         break;
    //         case FieldType::SHORT:
    //             int16_t i16 = Unbox<int16_t>(obj);
    //             c.Write(&i16 , sizeof(int16_t));
    //         break;
    //         case FieldType::INT:
    //             int32_t i32 = Unbox<int32_t>(obj);
    //             c.Write(&i32 , sizeof(int32_t));
    //         break;
    //         case FieldType::LONG:
    //             int64_t i64 = Unbox<int64_t>(obj);
    //             c.Write(&i64 , sizeof(int64_t));
    //         break;
    //         case FieldType::UBYTE:
    //             uint8_t ui8 = Unbox<uint8_t>(obj);
    //             c.Write(&ui8 , sizeof(uint8_t));
    //         break;
    //         case FieldType::USHORT:
    //             uint16_t ui16 = Unbox<uint16_t>(obj);
    //             c.Write(&ui16 , sizeof(uint16_t));
    //         break;
    //         case FieldType::UINT:
    //             uint32_t ui32 = Unbox<uint32_t>(obj);
    //             c.Write(&ui32 , sizeof(uint32_t));
    //         break;
    //         case FieldType::ULONG:
    //             uint64_t ui64 = Unbox<uint64_t>(obj);
    //             c.Write(&ui64 , sizeof(uint64_t));
    //         break;
    //         case FieldType::FLOAT:
    //             float f = Unbox<float>(obj);
    //             c.Write(&f , sizeof(float));
    //         break;
    //         case FieldType::DOUBLE:
    //             double d = Unbox<double>(obj);
    //             c.Write(&d , sizeof(double));
    //         break;
    //         case FieldType::STRING:
    //             std::string str = MonoStringToStr(static_cast<MonoString*>(obj));
    //             c.Allocate(str.size() + 1);
    //             c.ZeroOut();
    //             c.Write(str.data() , str.size());
    //         break;
    //         case FieldType::VECTOR2:
    //             glm::vec2 val = Unbox<glm::vec2>(obj);
    //             c.Write(glm::value_ptr(val) , sizeof(glm::vec2));
    //         break;
    //         case FieldType::VECTOR3:
    //             glm::vec3 val3 = Unbox<glm::vec3>(obj);
    //             c.Write(glm::value_ptr(val3) , sizeof(glm::vec3));
    //         break;
    //         case FieldType::VECTOR4:
    //             glm::vec4 val4 = Unbox<glm::vec4>(obj);
    //             c.Write(glm::value_ptr(val4) , sizeof(glm::vec4));
    //         break;
    //     }
    // }

    ScriptField* ScriptMap::GetFieldByName(const ScriptObject* obj , const std::string& name) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to access field from null object");
        
        uint32_t id = Hash::FNV32(obj->name + ":" + name);
        if (script_map->script_fields.find(id) != script_map->script_fields.end())
            return &script_map->script_fields[id];

        return nullptr;
    }

    ScriptField* ScriptMap::GetFieldByID(const ScriptObject* obj , UUID32 id) {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to access field from null object");

        if (script_map->script_fields.find(id) != script_map->script_fields.end())
            return &script_map->script_fields[id];

        return nullptr;
    }

    MonoClass* ScriptMap::GetFieldClass(FieldType field) {
        switch (field) {
            case FieldType::BOOL: return YE_CACHED_CLASS("System.Boolean")->klass;
            case FieldType::CHAR: return YE_CACHED_CLASS("System.Char")->klass;
            case FieldType::BYTE: return YE_CACHED_CLASS("System.Int8")->klass;
            case FieldType::SHORT: return YE_CACHED_CLASS("System.Int16")->klass;
            case FieldType::INT: return YE_CACHED_CLASS("System.Int32")->klass;
            case FieldType::LONG: return YE_CACHED_CLASS("System.Int64")->klass;
            case FieldType::UBYTE: return YE_CACHED_CLASS("System.UInt8")->klass;
            case FieldType::USHORT: return YE_CACHED_CLASS("System.UInt16")->klass;
            case FieldType::UINT: return YE_CACHED_CLASS("System.UInt32")->klass;
            case FieldType::ULONG: return YE_CACHED_CLASS("System.UInt64")->klass;
            case FieldType::FLOAT: return YE_CACHED_CLASS("System.Single")->klass;
            case FieldType::DOUBLE: return YE_CACHED_CLASS("System.Double")->klass;
            case FieldType::STRING: return YE_CACHED_CLASS("System.String")->klass;
            case FieldType::VECTOR2: return YE_CACHED_CLASS("YE.Vec2")->klass;
            case FieldType::VECTOR3: return YE_CACHED_CLASS("YE.Vec3")->klass;
            case FieldType::VECTOR4: return YE_CACHED_CLASS("YE.Vec4")->klass;
            case FieldType::ENTITY: return YE_CACHED_CLASS("YE.Entity")->klass;

            /// \todo implement the rest
            // case FieldType::QUATERNION: return YE_CACHED_CLASS("YE.Quaternion")->klass;
            // case FieldType::MATRIX4: return YE_CACHED_CLASS("YE.Mat4")->klass;
            // case FieldType::ASSET: return YE_CACHED_CLASS("YE.Asset")->klass;
            // case FieldType::PREFAB: return YE_CACHED_CLASS("YE.Prefab")->klass;
            // case FieldType::MESH: return YE_CACHED_CLASS("YE.Mesh")->klass;
            // case FieldType::STATICMESH: return YE_CACHED_CLASS("YE.StaticMesh")->klass;
            // case FieldType::MATERIAL: return YE_CACHED_CLASS("YE.Material")->klass;
            // case FieldType::PHYSICSMATERIAL: return YE_CACHED_CLASS("YE.PhysicsMaterial")->klass;
            // case FieldType::TEXTURE: return YE_CACHED_CLASS("YE.Texture")->klass;
            default: break;
        }
        return nullptr;
    }

    MonoObject* ScriptMap::GetFieldValueObj(MonoObject* klass , ScriptField* field) {
        MonoClass* obj_class = mono_object_get_class(klass);

        MonoObject* field_obj = nullptr;
        MonoObject* exception = nullptr;

        if (field->is_property) {
            MonoProperty* prop = mono_class_get_property_from_name(obj_class , field->name.c_str());
            field_obj = mono_property_get_value(prop , klass , nullptr , &exception);
            CHECK_MONO_EXCEPTION(exception);
        } else {
            MonoClassField* f = mono_class_get_field_from_name(obj_class , field->name.c_str());
            field_obj = mono_field_get_value_object(mono_domain_get() , f , klass);
        }

        return field_obj;
    }
    
    bool ScriptMap::CheckFieldVisibility(const ScriptObject* object , const ScriptField* field) {
        if (field->HasFlag(FieldAccess::PUBLIC))
            return true;

        if (field->HasFlag(FieldAccess::PRIVATE)) {
            for (auto& f : object->fields) {
                if (f == field->IDU32)
                    return true;
            }
        }

        if (field->HasFlag(FieldAccess::PROTECTED)) {
            /// \todo iterate through parents to see if inherited value
        }

        if (field->HasFlag(FieldAccess::INTERNAL)) {
            /// \todo handle this case??
        }
        
        return false;
    }

    void ScriptMap::Generate() {
        YE_CRITICAL_ASSERTION(script_map == nullptr , "Generating script map twice!");
        script_map = ynew ScriptObjectMap;
        StoreCoreClasses();
    }

    void ScriptMap::LoadProjectTypes() {
        MonoImage* img = ScriptEngine::Instance()->GetProjectScriptData()->image;
        const MonoTableInfo* table = mono_image_get_table_info(img , MONO_TABLE_TYPEDEF);
        uint32_t rows = mono_table_info_get_rows(table);

        for (uint32_t i = 0; i < rows; ++i) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(table , i , cols , MONO_TYPEDEF_SIZE);

            const char* name_space = mono_metadata_string_heap(img , cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(img , cols[MONO_TYPEDEF_NAME]);
            std::string name_str{ name };

            if (name_str == "<Module>") continue;

            MonoClass* klass = mono_class_from_name(
                                    ScriptEngine::Instance()->project_script_data.image ,
                                    name_space , name
                                );
            
            MonoType* ctype = mono_class_get_type(klass);
            ScriptObject obj{};
            obj.name = name;
            obj.IDU32 = Hash::FNV32(obj.name);
            uint32_t alignment = 0;

            obj.size = mono_class_value_size(klass , &alignment);
            obj.klass = klass;
            obj.is_custom = true;

            script_map->script_objs[obj.IDU32] = obj;
            StoreClassMethods(obj);
            StoreClassFields(obj);
            StoreClassProperties(obj);
        }
    }
    
    
    void ScriptMap::UnloadProjectTypes() {
        for (auto& [id , obj] : script_map->script_objs) {
            if (obj.is_custom) {
                for (auto& field : obj.fields)
                    script_map->script_fields.erase(field);
                obj.fields.clear();

                for (auto& method : obj.methods)
                    script_map->script_methods.erase(method);
                obj.methods.clear();
            }
        }
    }

    void ScriptMap::Destroy() {
        script_map->script_objs.clear();
        script_map->script_fields.clear();
        script_map->script_methods.clear();
        if (script_map == nullptr) return;
        ydelete script_map;
        script_map = nullptr;
    }

#ifdef YE_DEBUG_BUILD
    void ScriptMap::PrintAll() {
        YE_CRITICAL_ASSERTION(script_map != nullptr , "Attempting to access script map before creation");
        for (auto& obj : script_map->script_objs) {
            YE_INFO("[Object {0}] :: {1}" , obj.first.uuid , obj.second.name);
        }
        for (auto& field : script_map->script_fields) {
            YE_INFO("[Field {0}] :: {1}" , field.first.uuid , field.second.name);
        }
        for (auto& method : script_map->script_methods) {
            YE_INFO("[{0} Methods] :: " , method.second.size());
            for (auto& m : method.second) {
                YE_INFO("    {0} - {1}" , m.IDU32.uuid , m.name);
            }
        }
    }
#endif

}