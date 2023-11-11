#include "scripting/script_engine.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#if YE_PLATFORM_WIN
    #include <ShlObj.h>
    #include <minwindef.h>
#endif

#include <mono/metadata/mono-debug.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <spdlog/fmt/fmt.h>

#include "log.hpp"
#include "core/filesystem.hpp"
#include "scene/components.hpp"
#include "scripting/garbage_collector.hpp"

#define DEBUG_SCRIPTS 0

namespace YE {

    enum AsmVersionIndex {
        MAJOR = 0 ,
        MINOR = 1 ,
        PATCH = 2 ,
        REVISION = 3
    };
    
    void ScriptEngine::GetAssemblyProperties(MonoImage* img , AssemblyProperties& asm_properties) {
        ENTER_FUNCTION_TRACE_MSG("Assembly: {0}" , mono_image_get_name(img));

        const MonoTableInfo* table = mono_image_get_table_info(img , MONO_TABLE_ASSEMBLY);
        uint32_t cols[MONO_TABLE_ASSEMBLY];
        mono_metadata_decode_row(table , 0 , cols , MONO_ASSEMBLY_SIZE);

        asm_properties.name = mono_metadata_string_heap(img , cols[MONO_ASSEMBLY_NAME]);
        asm_properties.version[AsmVersionIndex::MAJOR] = cols[MONO_ASSEMBLY_MAJOR_VERSION] > 0 ? 
                                                            cols[MONO_ASSEMBLY_MAJOR_VERSION] : 1;
        asm_properties.version[AsmVersionIndex::MINOR] = cols[MONO_ASSEMBLY_MINOR_VERSION];
        asm_properties.version[AsmVersionIndex::PATCH] = cols[MONO_ASSEMBLY_BUILD_NUMBER];
        asm_properties.version[AsmVersionIndex::REVISION] = cols[MONO_ASSEMBLY_REV_NUMBER];

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::GetReferencedAssemblies(MonoImage* img , AssemblyProperties& asm_properties) {
        ENTER_FUNCTION_TRACE_MSG("Assembly: {0}" , mono_image_get_name(img));

        const MonoTableInfo* table = mono_image_get_table_info(img , MONO_TABLE_ASSEMBLYREF);
        uint32_t rows = mono_table_info_get_rows(table);

        for (uint32_t i = 0; i < rows; ++i) {
            AssemblyProperties asm_ref;
            uint32_t cols[MONO_ASSEMBLYREF_SIZE];
            mono_metadata_decode_row(table , i , cols , MONO_ASSEMBLYREF_SIZE);

            asm_ref.name = std::string{ mono_metadata_string_heap(img , cols[MONO_ASSEMBLYREF_NAME]) };
            asm_ref.version[AsmVersionIndex::MAJOR] = cols[MONO_ASSEMBLYREF_MAJOR_VERSION];
            asm_ref.version[AsmVersionIndex::MINOR] =cols[MONO_ASSEMBLYREF_MINOR_VERSION];
            asm_ref.version[AsmVersionIndex::PATCH] =cols[MONO_ASSEMBLYREF_BUILD_NUMBER];
            asm_ref.version[AsmVersionIndex::REVISION] =cols[MONO_ASSEMBLYREF_REV_NUMBER];

            asm_properties.referenced_assemblies.push_back(asm_ref);
        }

        EXIT_FUNCTION_TRACE();
    }

    MonoAssembly* ScriptEngine::LoadCSharpAsm(MonoImage *& img , const std::string& filepath , bool internal) {
        ENTER_FUNCTION_TRACE_MSG("Assembly: {0}" , filepath);

        uint32_t fsize = 0;
        std::vector<char> bytes = Filesystem::ReadFileAsSBytes(filepath);

        YE_CRITICAL_ASSERTION(bytes.size() != 0 , "Failed to read C# assembly : {0}" , filepath);

        MonoImageOpenStatus status;
        img = mono_image_open_from_data_full(bytes.data() , static_cast<uint32_t>(bytes.size()) , 1 , &status , 0);

        YE_CRITICAL_ASSERTION(status == MONO_IMAGE_OK , "Failed to load image from C# assembly : {0}" , filepath);

#if DEBUG_SCRIPTS
        LoadDebugImage(img , filepath);
#endif

        MonoAssembly* assembly = mono_assembly_load_from_full(img , filepath.c_str() , &status , 0);

        if (internal) {
            const char* name = mono_image_get_name(img);
            printf("Loaded internal assembly : %s\n" , name);
        }

        EXIT_FUNCTION_TRACE();
        return assembly;
    }
    
    void ScriptEngine::InitializeScriptDebugging(uint16_t port) {
        ENTER_FUNCTION_TRACE_MSG("Port: {0}" , port);

        if (!std::filesystem::directory_entry("logs").exists())
            std::filesystem::create_directory("logs");

        std::string addr = "127.0.0.1:" + std::to_string(port);
        std::string log = "logs/mono-debug.log";

        std::string arg1 = "--debugger-agent=transport=dt_socket,address=" + addr + ",server=y,suspend=n,loglevel=3,logfile=" + log;
        std::string arg2 = "--soft-breakpoints";
        const char* argv[2] = {
            arg1.c_str() , arg2.c_str()
        };

        mono_jit_parse_options(2 , const_cast<char**>(argv));
        mono_debug_init(MONO_DEBUG_FORMAT_MONO);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::CreateDebugDomains() {
        ENTER_FUNCTION_TRACE();

        mono_debug_domain_create(root_domain);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::LoadDebugImage(MonoImage* img , const std::string& filepath) {
        ENTER_FUNCTION_TRACE_MSG("Assembly: {0}" , filepath);

        std::string pdb_path = filepath.substr(0 , filepath.find_last_of('.')) + ".pdb";
        
        std::vector<char> bytes = Filesystem::ReadFileAsSBytes(pdb_path);
        
        mono_debug_open_image_from_memory(
            img , 
            reinterpret_cast<const mono_byte*>(bytes.data()) ,
            static_cast<uint32_t>(bytes.size())
        );

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::LoadInternalScripts() {
        ENTER_FUNCTION_TRACE();

        internal_script_data.assembly = LoadCSharpAsm(internal_script_data.image , internal_modules_path);
        GetAssemblyProperties(internal_script_data.image , core_asm_properties);
        GetReferencedAssemblies(internal_script_data.image , core_asm_properties);                                                                                                       
    
        EXIT_FUNCTION_TRACE();
    }    
    
    void ScriptEngine::LoadProjectScripts() {
        ENTER_FUNCTION_TRACE();

        project_script_data.assembly = LoadCSharpAsm(project_script_data.image , project_modules_path);
        GetAssemblyProperties(project_script_data.image , project_asm_properties);
        GetReferencedAssemblies(project_script_data.image , project_asm_properties);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::UnloadProjectScripts() {
        ENTER_FUNCTION_TRACE();

        mono_image_close(project_script_data.image);
        project_script_data.image = nullptr;

        mono_assembly_close(project_script_data.assembly);
        project_script_data.assembly = nullptr;

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::GetField(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Field: {1}" , obj->name , field->name);

        MonoClassField* mfield = mono_class_get_field_from_name(obj->klass , field->name.c_str());
        YE_CRITICAL_ASSERTION(mfield != nullptr , "Failed to retrieve field: {0} from class: {1}" , field->name , obj->name);
        
        MonoObject* mobj = mono_field_get_value_object(app_domain , mfield , instance);
        YE_CRITICAL_ASSERTION(mobj != nullptr , "Failed to retrieve field: {0} from class: {1}" , field->name , obj->name);

        if (field->type == FieldType::STRING) {
            MonoString* mstr = (MonoString*)mobj;
            std::string str = ScriptUtils::MonoStringToStr(mstr);
            value->handle = static_cast<FieldHandle>(mstr);
            value->size = str.size();
            return;
        }
        
        /// \todo add support for arrays

        /// \todo Add size checking for types that require it

        value->handle = mono_object_unbox(mobj);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::GetProperty(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Property: {1}" , obj->name , field->name);

        MonoProperty* mprop = mono_class_get_property_from_name(obj->klass , field->name.c_str());
        YE_CRITICAL_ASSERTION(mprop != nullptr , "Failed to retrieve property: {0} from class: {1}" , field->name , obj->name);

        MonoObject* exc = nullptr;
        MonoObject* result = mono_property_get_value(mprop , instance , nullptr , &exc);
        CHECK_MONO_EXCEPTION(exc);

        if (field->type == FieldType::STRING) {
            MonoString* mstr = (MonoString*)result;
            std::string str = ScriptUtils::MonoStringToStr(mstr);
            value->handle = static_cast<FieldHandle>(mstr);
            value->size = str.length();
            return;
        }

        if (field->type == FieldType::ENTITY) {
            ScriptObject* ent = ScriptMap::GetClassByName("YE.Entity");
            YE_CRITICAL_ASSERTION(ent != nullptr , "Failed to retrieve entity class");

            Field f;
            GetObjField(ent , result , "Id" , &f);

            value->handle = f.handle;
            return;
        }
        
        /// \todo add support for arrays

        /// \todo Add size checking for types that require it

        value->handle = mono_object_unbox(result);
        
        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::SetField(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Field: {1}" , obj->name , sf->name);

        MonoClassField* mfield = mono_class_get_field_from_name(obj->klass , sf->name.c_str());
        YE_CRITICAL_ASSERTION(mfield != nullptr , "Failed to retrieve field: {0} from class: {1}" , sf->name , obj->name);

        if (sf->type == FieldType::STRING) {
            std::cout << "size of string: " << value->size << "\n";
            std::string str(static_cast<char*>(value->handle) , value->size);
            MonoString* mstr = mono_string_new(app_domain , str.c_str());
            mono_field_set_value(instance , mfield , mstr);
            return;
        }
        
        /// \todo add support for arrays

        /// \todo Add size checking for types that require it

        mono_field_set_value(instance , mfield , value->handle);
        
        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::SetProperty(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Property: {1}" , obj->name , sf->name);

        MonoProperty* mprop = mono_class_get_property_from_name(obj->klass , sf->name.c_str());
        YE_CRITICAL_ASSERTION(mprop != nullptr , "Failed to retrieve property: {0} from class: {1}" , sf->name , obj->name);

        if (sf->type == FieldType::STRING) {
            std::string str(static_cast<char*>(value->handle) , value->size);
            MonoString* mstr = mono_string_new(app_domain , str.c_str());

            MonoObject* handled_obj = ScriptGC::GetHandleObject(handle);
            YE_CRITICAL_ASSERTION(handled_obj != nullptr , "Failed to retrieve handle object for entity");

            FieldHandle param = mstr;
            MonoObject* exc = nullptr;
            mono_property_set_value(mprop , handled_obj , &param , &exc);

            CHECK_MONO_EXCEPTION(exc);
            EXIT_FUNCTION_TRACE();
            return;
        }

        if (sf->type == FieldType::ENTITY) {
            UUID* id = (UUID*)value->handle;

            Entity* ent = internal_state->scene_context->GetEntity(*id);
            if (ent == nullptr) {
                ENGINE_WARN("Failed to find entity with id: {0} | Could not set entity parent" , id->uuid);
                EXIT_FUNCTION_TRACE();
                return;
            }

            MonoObject* execute_instance = nullptr;

            if (!ent->HasComponent<components::Script>()) {
                ScriptObject* ent = CreateObject(execute_instance , "YE.Entity");

                ScriptMethod* ctor = ScriptMap::GetMethod(ent , ".ctor" , 1);
                YE_CRITICAL_ASSERTION(ctor != nullptr , "Failed to find constructor for script object");

                ParamHandle params[] = { &id->uuid };
                InvokeMethod(execute_instance , ctor , params);
            } else {
                auto& script = ent->GetComponent<components::Script>();
                execute_instance = script.instance;
            }

            MonoObject* object = ScriptGC::GetHandleObject(handle);
            YE_CRITICAL_ASSERTION(object != nullptr , "Failed to retrieve handle object for entity");

            FieldHandle param = execute_instance;
            MonoObject* exc = nullptr;
            mono_property_set_value(mprop , object , &param , &exc);
            CHECK_MONO_EXCEPTION(exc);

            EXIT_FUNCTION_TRACE();
            return;
        }

        /// \todo add support for arrays

        /// \todo Add size checking for types that require it
        MonoObject* handled_obj = ScriptGC::GetHandleObject(handle);
        YE_CRITICAL_ASSERTION(handled_obj != nullptr , "Failed to retrieve handle object for entity");

        MonoObject* exc = nullptr;
        FieldHandle param = value->handle;
        mono_property_set_value(mprop , handled_obj , &param , &exc);

        CHECK_MONO_EXCEPTION(exc);
        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::ShutdownMono() {
        ENTER_FUNCTION_TRACE();
        mono_domain_set(mono_get_root_domain() , false);
        
        // mono_domain_unload(app_domain);
        // root_domain = nullptr;

#if DEBUG_SCRIPTS
        mono_debug_cleanup();
#endif

        mono_jit_cleanup(root_domain);
        root_domain = nullptr;

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::PrintCoreAsmInfo() {
        ENTER_FUNCTION_TRACE();

        MonoImage* image = mono_assembly_get_image(internal_script_data.assembly);
        const MonoTableInfo* table = mono_image_get_table_info(image , MONO_TABLE_TYPEDEF);
        int32_t rows = mono_table_info_get_rows(table);

        for (int32_t i = 0; i < rows; ++i) {
            uint32_t cols[MONO_TYPEDEF_SIZE];

            mono_metadata_decode_row(table , i , cols , MONO_TYPEDEF_SIZE);
            const char* name_space = mono_metadata_string_heap(image , cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image , cols[MONO_TYPEDEF_NAME]);

            printf("%s::%s\n" , name_space , name);
        }
        
        EXIT_FUNCTION_TRACE();
    }

    ScriptEngine* ScriptEngine::instance = nullptr;

    ScriptEngine* ScriptEngine::Instance() {
        if (instance == nullptr) {
            instance = new ScriptEngine;
        }
        return instance;
    }

    ScriptObject* ScriptEngine::CreateObject(MonoObject *& instance , const std::string& name) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , name);

        ScriptObject* obj = ScriptMap::GetClassByName(name);
        if (obj == nullptr) {
            ENGINE_WARN("Failed to create script object instance");
            return nullptr;
        }

        instance = mono_object_new(app_domain , obj->klass);
        YE_CRITICAL_ASSERTION(instance != nullptr , "Failed to create script object instance");

        InstantiateObject(obj , instance);

        EXIT_FUNCTION_TRACE();
        return obj;
    }

    void ScriptEngine::DestroyObject(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , obj->name);

        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to destroy null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to destroy null instance");

        InvokeDestroy(obj , instance , handle);

        ScriptGC::FreeHandle(handle);
        handle = nullptr;
        instance = nullptr;

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::InstantiateObject(ScriptObject* obj , MonoObject* instance) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , obj->name);

        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to instantiate null object");
        mono_runtime_object_init(instance);

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::GetObjField(ScriptObject* obj , MonoObject* instance , const std::string& field , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Field: {1}" , obj->name , field);

        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to get field on null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to get field object with null instance");
        YE_CRITICAL_ASSERTION(value != nullptr , "Attempting to get null field");

        ScriptField* sf = ScriptMap::GetFieldByName(obj , field);
        if (sf == nullptr) {
            ENGINE_ERROR("Failed to find field: {0} on object: {1}" , field , obj->name);
            EXIT_FUNCTION_TRACE();
            return;
        }

        if (sf->is_property) {
            GetProperty(obj , sf , instance , value);
        } else {
            GetField(obj , sf , instance , value);
        }

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::SetObjField(ScriptObject* obj , MonoObject* instance , GCHandle handle , const std::string& field , Field* value) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0} | Field: {1}" , obj->name , field);

        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to set field on null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to set field object with null instance");
        YE_CRITICAL_ASSERTION(value != nullptr , "Attempting to set field with null param");

        ScriptField* sf = ScriptMap::GetFieldByName(obj , field);
        if (sf == nullptr) {
            ENGINE_ERROR("Failed to find field: {0} on object: {1}" , field , obj->name);
            EXIT_FUNCTION_TRACE();
            return;
        }

        if (sf->is_property) {
            SetProperty(obj , sf , instance , handle , value);
        } else {
            SetField(obj , sf , instance , handle , value);
        }

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::InitializeEntity(Entity* entity , uint32_t num_ctor_params , ParamHandle* params) {
        ENTER_FUNCTION_TRACE_MSG("Entity: {0}" , entity->GetComponent<components::ID>().name);

        if (!entity->HasComponent<components::Script>()) {
            EXIT_FUNCTION_TRACE();
            return;
        }

        auto& script = entity->GetComponent<components::Script>();
        auto& eid = entity->GetComponent<components::ID>();

        if (!script.bound){
            ENGINE_WARN("Attempting to initialize unbound script on entity :: [{0} , {1}]" , script.object->name , eid.name);
        }

        for (auto& field : script.object->fields) {
            ScriptField* field = ScriptMap::GetFieldByID(script.object , field->IDU32);
            if (field == nullptr) {
                ENGINE_ERROR("Failed to find script field: {0} when trying to initialize: {1}" , field->name , script.object->name);
                continue;
            }

            if (!field->HasFlag(FieldAccess::PUBLIC))
                continue;

            if (field->IsArray()) {
                YE_CRITICAL_ASSERTION(false , "TODO: Implement array fields for scripts");
                // internal_state->entity_field_map[eid.id][field->IDU32] = std::make_unique<ArrayField>(field);
            } else {
                internal_state->entity_field_map[eid.id][field->IDU32] = std::make_unique<Field>();
            }
        }
        
        script.handle = ScriptGC::NewHandle(script.instance , false);
        script.active = true;

        YE_CRITICAL_ASSERTION(script.handle != nullptr , "Failed to create handle for script: {0}" , script.object->name);

        MonoObject* object = ScriptGC::GetHandleObject(script.handle);

        ScriptMethod* ctor = ScriptMap::GetMethod(script.object , ".ctor" , num_ctor_params);
        YE_CRITICAL_ASSERTION(ctor != nullptr , "Failed to find constructor for script object");

        InvokeMethod(object , ctor , params);

        InvokeCreate(script.object , script.instance , script.handle);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::ActivateEntity(Entity* entity) {
        ENTER_FUNCTION_TRACE_MSG("Entity: {0}" , entity->GetComponent<components::ID>().name);

        YE_CRITICAL_ASSERTION(false , "TODO: Implement ScriptEngine::EntityRuntimeInit()");
        // auto script = entity.GetComponent<components::Script>();

        // if (!script.bound)
        //     return;

        // if (script.active)
        //     return;

        // GCHandle handle = ScriptGC::NewHandle(script.object->instance , false);
        // entity.GetComponent<components::Script>().object->handle = handle;
        // internal_state->entity_handles[entity.GetComponent<components::ID>().id] = handle;

        // for (auto field : script.object->fields) {
        //     UUID eid = entity.GetComponent<components::ID>().id;

        //     if (internal_state->entity_field_map.find(eid) != internal_state->entity_field_map.end())
        //         internal_state->entity_field_map[eid][field]->SetRuntimeParent(handle);
        // }

        // InstantiateObject(script.object);
        // InvokeCreate(script.object);

        // entity.GetComponent<components::Script>().active = true;

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::DeactivateEntity(Entity* entity, bool erase) {
        ENTER_FUNCTION_TRACE_MSG("Entity: {0}" , entity->GetComponent<components::ID>().name);

        // if (!entity.HasComponent<components::Script>())
        //     return;

        // auto& script = entity.GetComponent<components::Script>();
        // UUID eid = entity.GetComponent<components::ID>().id;

        // if (!script.bound || script.active)
        //     return;

        // if (erase && internal_state->context_entities->find(eid) != internal_state->context_entities.end())
        //     internal_state->context_entities->erase(eid);

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::DestroyEntity(Entity* entity) {
        ENTER_FUNCTION_TRACE_MSG("Entity: {0}" , entity->GetComponent<components::ID>().name);

        if (!entity->HasComponent<components::Script>()) {
            EXIT_FUNCTION_TRACE();
            return;
        }

        auto& script = entity->GetComponent<components::Script>();

        if (!script.bound || !script.active) {
            EXIT_FUNCTION_TRACE();
            return;
        }

        InvokeDestroy(script.object , script.instance , script.handle);

        // do something with fields
        ScriptGC::FreeHandle(script.handle);
        script.handle = nullptr;
        script.bound = false;
        script.active = false;

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::InvokeCreate(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , obj->name);

        YE_CRITICAL_ASSERTION(obj->klass != nullptr , "Attempting to call Create on null class");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to call Create on null object");

        MonoObject* exc = nullptr;
        MonoMethod* method = mono_class_get_method_from_name(obj->klass , "Create" , 0);
        YE_CRITICAL_ASSERTION(method != nullptr , "Failed to retrieve Create method from class: {0}" , mono_class_get_name(obj->klass));

        MonoMethod* vmethod = mono_object_get_virtual_method(instance , method);
        YE_CRITICAL_ASSERTION(vmethod != nullptr , "Failed to retrieve Create method from class: {0}" , mono_class_get_name(obj->klass));

        MonoObject* object = ScriptGC::GetHandleObject(handle);
        mono_runtime_invoke(vmethod , object , nullptr , &exc);
        CHECK_MONO_EXCEPTION(exc);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::InvokeUpdate(ScriptObject* obj , MonoObject* instance , GCHandle handle , float delta_time) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , obj->name);

        YE_CRITICAL_ASSERTION(obj->klass != nullptr , "Attempting to call Update on null class");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to call Update on null object");

        MonoObject* exc = nullptr;
        MonoMethod* method = mono_class_get_method_from_name(obj->klass , "Update" , 1);
        YE_CRITICAL_ASSERTION(method != nullptr , "Failed to retrieve Update method from class: {0}" , mono_class_get_name(obj->klass));

        MonoMethod* vmethod = mono_object_get_virtual_method(instance , method);
        YE_CRITICAL_ASSERTION(vmethod != nullptr , "Failed to retrieve Update method from class: {0}" , mono_class_get_name(obj->klass));

        ParamHandle params[] = { &delta_time };
        MonoObject* object = ScriptGC::GetHandleObject(handle);

        mono_runtime_invoke(vmethod , object , params , &exc);
        CHECK_MONO_EXCEPTION(exc);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::InvokeDestroy(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
        ENTER_FUNCTION_TRACE_MSG("Object: {0}" , obj->name);

        YE_CRITICAL_ASSERTION(obj->klass != nullptr , "Attempting to call Destroy on null class");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to call Destroy on null object");

        MonoObject* exc = nullptr;
        MonoMethod* method = mono_class_get_method_from_name(obj->klass , "Destroy" , 0);
        YE_CRITICAL_ASSERTION(method != nullptr , "Failed to retrieve Destroy method from class: {0}" , mono_class_get_name(obj->klass));

        MonoMethod* vmethod = mono_object_get_virtual_method(instance , method);
        YE_CRITICAL_ASSERTION(vmethod != nullptr , "Failed to retrieve Destroy method from class: {0}" , mono_class_get_name(obj->klass));

        MonoObject* object = ScriptGC::GetHandleObject(handle);
        mono_runtime_invoke(vmethod , object , nullptr , &exc);
        CHECK_MONO_EXCEPTION(exc);

        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::InvokeMethod(MonoObject* obj , ScriptMethod* method , ParamHandle* params) {
        ENTER_FUNCTION_TRACE_MSG("Method: {}" , method->name);

        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to call method on null object");
        YE_CRITICAL_ASSERTION(method != nullptr , "Attempting to call null method");
        YE_CRITICAL_ASSERTION(method->method != nullptr , "Attempting to call null method");

        if (method->param_count > 0)
            YE_CRITICAL_ASSERTION(params != nullptr , "Attempting to call method with null params"
        );

        MonoObject* exception = nullptr;
        mono_runtime_invoke(method->method , obj , params , &exception);
        CHECK_MONO_EXCEPTION(exception);

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::Initialize() {
        ENTER_FUNCTION_TRACE();

        YE_CRITICAL_ASSERTION(!initialized , "Attempting to initialize script engine twice");

        mono_path = Filesystem::GetMonoPath();
        mono_config_path = Filesystem::GetMonoConfigPath();
        internal_modules_path = Filesystem::GetInternalModulesPath();
        project_modules_path = Filesystem::GetProjectModulesPath();

        YE_CRITICAL_ASSERTION(std::filesystem::exists(mono_path) , "Failed to find mono path: {0}" , mono_path);
        YE_CRITICAL_ASSERTION(std::filesystem::exists(mono_config_path) , "Failed to find mono config path: {0}" , mono_config_path);
        YE_CRITICAL_ASSERTION(std::filesystem::exists(internal_modules_path) , "Failed to find internal modules path: {0}" , internal_modules_path);
        YE_CRITICAL_ASSERTION(std::filesystem::exists(project_modules_path) , "Failed to find project modules path: {0}" , project_modules_path);

        root_domain_name = "YERootScriptDomain";
        app_domain_name = "YEAppScriptDomain";
        std::string current_dir = Filesystem::GetCWD();

#ifdef YE_PLATFORM_WIN
        std::replace(current_dir.begin() , current_dir.end() , '/' , '\\'); 
#endif

        mono_set_dirs(mono_path.data() , mono_config_path.data());

#if DEBUG_SCRIPTS
        /// \todo Make port number configurable
        InitializeScriptDebugging(8080);
#endif     

        root_domain = mono_jit_init(root_domain_name.c_str());
        YE_CRITICAL_ASSERTION(root_domain != nullptr , "Failed to initialize Mono domain");


        app_domain = mono_domain_create_appdomain(app_domain_name.data() , nullptr);
        YE_CRITICAL_ASSERTION(app_domain != nullptr , "Failed to create Mono app domain");

#if DEBUG_SCRIPTS
        CreateDebugDomains();
#endif

        mono_thread_set_main(mono_thread_current());
        mono_domain_set(app_domain , true);
        
        LoadInternalScripts();
        ScriptGlue::BindAssembly();
        ScriptMap::Generate();
        ScriptGC::Initialize();

        initialized = true;
        internal_state = std::make_unique<EngineState>();

        EXIT_FUNCTION_TRACE();
    }
    
    uint32_t ScriptEngine::BuildProjectModules(std::filesystem::path module_proj_path , std::string module_path) {
        ENTER_FUNCTION_TRACE_MSG("Project Modules Path: {0}" , module_proj_path.string());

#if YE_PLATFORM_WIN
        TCHAR program_files_path_buffer[MAX_PATH];
		SHGetSpecialFolderPath(0, program_files_path_buffer, CSIDL_PROGRAM_FILES, FALSE);
		std::filesystem::path ms_build = std::filesystem::path(program_files_path_buffer) / 
                                            "Microsoft Visual Studio" / "2022" / "Community" / 
                                            "Msbuild" / "Current" / "Bin" / "MSBuild.exe";

        std::replace(module_path.begin() , module_path.end() , '/' , '\\');

        std::filesystem::path build_path = std::filesystem::path(module_path) / module_proj_path.stem();
        build_path += ".csproj";

        std::string command = fmt::format(
            "\"\"{}\" \"{}\" /p:Configuration=Debug\"", 
            ms_build.string(), build_path.string()
        );

        EXIT_FUNCTION_TRACE();
        return system(command.c_str());
#else
        YE_CRITICAL_ASSERTION(false , "TODO: Implement BuildProjectModules() for non-windows platforms");
        EXIT_FUNCTION_TRACE();
        return 1;
#endif
    }
    
    void ScriptEngine::LoadProjectModules() {
        ENTER_FUNCTION_TRACE();

        YE_CRITICAL_ASSERTION(initialized , "Attempting to load project modules before initializing script engine");
        LoadProjectScripts();
        ScriptMap::LoadProjectTypes();

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::UnloadProjectModules() {
        ENTER_FUNCTION_TRACE();

        YE_CRITICAL_ASSERTION(initialized , "Attempting to unload project modules before initializing script engine");
        ScriptMap::UnloadProjectTypes();
        UnloadProjectScripts();

        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::ReloadProjectModules() {
        ENTER_FUNCTION_TRACE();

        std::filesystem::path internal_proj_path = Filesystem::GetInternalModulesPath();
        LOG_INFO("Internal modules path: {0}" , internal_proj_path.string());
        
        std::filesystem::path module_proj_path = Filesystem::GetProjectModulesPath();
        std::string module_path = Filesystem::GetModulePath();
        
        if (std::filesystem::is_empty(module_path)) {
            EXIT_FUNCTION_TRACE();
            return;
        }

        bool scene_check = scene_started;
        if (scene_check && scene_context_exists)
            StopScene();

        ScriptGC::Collect(true);
        UnloadProjectModules();
        ScriptGC::Shutdown();
        ScriptMap::Destroy();

        uint32_t command = BuildProjectModules(module_proj_path , module_path);

        LoadInternalScripts();
        ScriptMap::Generate();
        ScriptGC::Initialize();
        
        if (command == 0) {
            LoadProjectModules();

            if (!scene_context_exists) {
                EXIT_FUNCTION_TRACE();
                return;
            }

            for (auto& [id , entity] : *internal_state->context_entities) {
                if (!entity->HasComponent<components::Script>())
                    continue;
                auto& script = entity->GetComponent<components::Script>();
                script.Bind(script.class_name);
            }

            if (scene_check)
                StartScene();

            scripts_reloaded = true;

            EXIT_FUNCTION_TRACE();
            return;
        } else {
            try {
                uint32_t num_modules = 0;
                for (auto& entry : std::filesystem::directory_iterator(module_proj_path)) {
                    if (entry.path().extension() == ".dll")
                        ++num_modules;
                }
                if (num_modules > 0) {
                    ENGINE_ERROR("Failed to build project modules");
                    ENGINE_ERROR("Attempting to build modules again");

                    command = BuildProjectModules(module_proj_path , module_path);
                    if (command == 0) {
                        LOG_INFO("Attempted rebuild successful");
                       
                        LoadProjectModules();

                        if (!scene_context_exists) {
                            EXIT_FUNCTION_TRACE();
                            return;
                        }

                        for (auto& [id , entity] : *internal_state->context_entities) {
                            if (!entity->HasComponent<components::Script>())
                                continue;
                            auto& script = entity->GetComponent<components::Script>();
                            script.Bind(script.class_name);
                        }

                        if (scene_check)
                            StartScene();

                        scripts_reloaded = true;

                        EXIT_FUNCTION_TRACE();
                        return;
                    } else {
                        ENGINE_ERROR("Attempted rebuild failed");
                    }
                } else {
                    scripts_reloaded = true;

                    EXIT_FUNCTION_TRACE();
                    return;
                }
            } catch (std::filesystem::filesystem_error& e) {
                ENGINE_ERROR("Failed to read project modules directory: {0}" , e.what());
                scripts_reloaded = true;

                EXIT_FUNCTION_TRACE();
                return;
            }
        }

        EXIT_FUNCTION_TRACE();
        YE_CRITICAL_ASSERTION(false , "UNREACHABLE");
        return;
    }
    
    void ScriptEngine::SetSceneContext(Scene* scene) {
        ENTER_FUNCTION_TRACE();
        YE_CRITICAL_ASSERTION(scene != nullptr , "Attempting to set null scene context");

        internal_state->scene_context = scene;
        internal_state->context_entities = scene->Entities();

        scene_context_exists = true;
    }
    
    void ScriptEngine::StartScene() {
        ENTER_FUNCTION_TRACE();
        YE_CRITICAL_ASSERTION(initialized , "Attempting to start scene before initializing script engine");
        YE_CRITICAL_ASSERTION(internal_state->scene_context != nullptr , "Attempting to start scene with null scene context");

        for (auto& [id , entity] : *internal_state->context_entities) {
            if (!entity->HasComponent<components::Script>())
                continue;
            
            auto& script = entity->GetComponent<components::Script>();
            if (!script.bound)
                script.Bind(script.class_name);

            const uint32_t num_params = script.constructor_args.size();
            const ParamHandle* params = num_params > 0 ? 
                                script.constructor_args.data() : nullptr;
            InitializeEntity(entity , num_params , const_cast<ParamHandle*>(params));
        }

        scene_started = true;
        EXIT_FUNCTION_TRACE();
    }

    void ScriptEngine::StopScene() {
        ENTER_FUNCTION_TRACE();
        YE_CRITICAL_ASSERTION(initialized , "Attempting to stop scene before initializing script engine");
        YE_CRITICAL_ASSERTION(internal_state->scene_context != nullptr , "Attempting to stop scene with null scene context");

        for (auto& [id , entity] : *internal_state->context_entities) {
            if (!entity->HasComponent<components::Script>())
                continue;
            
            auto& script = entity->GetComponent<components::Script>();
            if (script.bound)
                script.Unbind();

            if (!script.active) {
                ENGINE_WARN(
                    "Attempting to stop inactive script on entity :: [{0} , {1}]" , 
                    script.object->name , entity->GetComponent<components::ID>().name
                );
                continue;
            }

            DestroyEntity(entity);
        }

        scene_started = false;
        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::Shutdown() {
        ENTER_FUNCTION_TRACE();
        
        ScriptGC::Shutdown();
        ScriptMap::Destroy();
        ScriptGlue::UnbindAssembly();
        ShutdownMono();
        initialized = false;
        
        EXIT_FUNCTION_TRACE();
    }
    
    void ScriptEngine::Cleanup() {
        ENTER_FUNCTION_TRACE();

        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }

        EXIT_FUNCTION_TRACE();
    }
}