#include "scripting/script_engine.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
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
        const MonoTableInfo* table = mono_image_get_table_info(img , MONO_TABLE_ASSEMBLY);
        uint32_t cols[MONO_TABLE_ASSEMBLY];
        mono_metadata_decode_row(table , 0 , cols , MONO_ASSEMBLY_SIZE);

        asm_properties.name = mono_metadata_string_heap(img , cols[MONO_ASSEMBLY_NAME]);
        asm_properties.version[AsmVersionIndex::MAJOR] = cols[MONO_ASSEMBLY_MAJOR_VERSION] > 0 ? 
                                                            cols[MONO_ASSEMBLY_MAJOR_VERSION] : 1;
        asm_properties.version[AsmVersionIndex::MINOR] = cols[MONO_ASSEMBLY_MINOR_VERSION];
        asm_properties.version[AsmVersionIndex::PATCH] = cols[MONO_ASSEMBLY_BUILD_NUMBER];
        asm_properties.version[AsmVersionIndex::REVISION] = cols[MONO_ASSEMBLY_REV_NUMBER];
    }

    void ScriptEngine::GetReferencedAssemblies(MonoImage* img , AssemblyProperties& asm_properties) {
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
    }

    MonoAssembly* ScriptEngine::LoadCSharpAsm(MonoImage *& img , const std::string& filepath , bool internal) {
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

        return assembly;
    }
    
    void ScriptEngine::InitializeScriptDebugging(uint16_t port) {
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
    }

    void ScriptEngine::CreateDebugDomains() {
        mono_debug_domain_create(root_domain);
    }

    void ScriptEngine::LoadDebugImage(MonoImage* img , const std::string& filepath) {
        std::string pdb_path = filepath.substr(0 , filepath.find_last_of('.')) + ".pdb";
        
        std::vector<char> bytes = Filesystem::ReadFileAsSBytes(pdb_path);
        
        mono_debug_open_image_from_memory(
            img , 
            reinterpret_cast<const mono_byte*>(bytes.data()) ,
            static_cast<uint32_t>(bytes.size())
        );
    }
    
    void ScriptEngine::LoadInternalScripts() {
        internal_script_data.assembly = LoadCSharpAsm(internal_script_data.image , internal_modules_path);
        GetAssemblyProperties(internal_script_data.image , core_asm_properties);
        GetReferencedAssemblies(internal_script_data.image , core_asm_properties);                                                                                                       
    }    
    
    void ScriptEngine::LoadProjectScripts() {
        project_script_data.assembly = LoadCSharpAsm(project_script_data.image , project_modules_path);
        GetAssemblyProperties(project_script_data.image , project_asm_properties);
        GetReferencedAssemblies(project_script_data.image , project_asm_properties);
    }

    void ScriptEngine::UnloadProjectScripts() {
        mono_image_close(project_script_data.image);
        project_script_data.image = nullptr;

        mono_assembly_close(project_script_data.assembly);
        project_script_data.assembly = nullptr;
    }
    
    void ScriptEngine::GetField(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value) {
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
    }

    void ScriptEngine::GetProperty(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value) {
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
    }

    void ScriptEngine::SetField(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value) {
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
    }

    void ScriptEngine::SetProperty(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value) {
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
            return;
        }

        if (sf->type == FieldType::ENTITY) {
            UUID* id = (UUID*)value->handle;

            Entity* ent = internal_state->scene_context->GetEntity(*id);
            if (ent == nullptr) {
                YE_WARN("Failed to find entity with id: {0} | Could not set entity parent" , id->uuid);
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
    }
    
    void ScriptEngine::ShutdownMono() {
        mono_domain_set(mono_get_root_domain() , false);
        
        // mono_domain_unload(app_domain);
        // root_domain = nullptr;

#if DEBUG_SCRIPTS
        mono_debug_cleanup();
#endif

        mono_jit_cleanup(root_domain);
        root_domain = nullptr;
    }
    
    void ScriptEngine::PrintCoreAsmInfo() {
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
    }

    ScriptEngine* ScriptEngine::instance = nullptr;

    ScriptEngine* ScriptEngine::Instance() {
        if (instance == nullptr) {
            instance = new ScriptEngine;
        }
        return instance;
    }

    ScriptObject* ScriptEngine::CreateObject(MonoObject *& instance , const std::string& name) {
        ScriptObject* obj = ScriptMap::GetClassByName(name);
        if (obj == nullptr) {
            YE_WARN("Failed to create script object instance");
            return nullptr;
        }

        instance = mono_object_new(app_domain , obj->klass);
        YE_CRITICAL_ASSERTION(instance != nullptr , "Failed to create script object instance");

        InstantiateObject(obj , instance);

        return obj;
    }

    void ScriptEngine::DestroyObject(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to destroy null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to destroy null instance");

        InvokeDestroy(obj , instance , handle);

        ScriptGC::FreeHandle(handle);
        handle = nullptr;
        instance = nullptr;
    }

    void ScriptEngine::InstantiateObject(ScriptObject* obj , MonoObject* instance) {
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to instantiate null object");
        mono_runtime_object_init(instance);
    }
    
    void ScriptEngine::GetObjField(ScriptObject* obj , MonoObject* instance , const std::string& field , Field* value) {
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to get field on null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to get field object with null instance");
        YE_CRITICAL_ASSERTION(value != nullptr , "Attempting to get null field");

        ScriptField* sf = ScriptMap::GetFieldByName(obj , field);
        if (sf == nullptr) {
            YE_ERROR("Failed to find field: {0} on object: {1}" , field , obj->name);
            return;
        }

        if (sf->is_property) {
            GetProperty(obj , sf , instance , value);
        } else {
            GetField(obj , sf , instance , value);
        }
    }

    void ScriptEngine::SetObjField(ScriptObject* obj , MonoObject* instance , GCHandle handle , const std::string& field , Field* value) {
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to set field on null object");
        YE_CRITICAL_ASSERTION(instance != nullptr , "Attempting to set field object with null instance");
        YE_CRITICAL_ASSERTION(value != nullptr , "Attempting to set field with null param");

        ScriptField* sf = ScriptMap::GetFieldByName(obj , field);
        if (sf == nullptr) {
            YE_ERROR("Failed to find field: {0} on object: {1}" , field , obj->name);
            return;
        }

        if (sf->is_property) {
            SetProperty(obj , sf , instance , handle , value);
        } else {
            SetField(obj , sf , instance , handle , value);
        }
    }

    void ScriptEngine::InitializeEntity(Entity* entity , uint32_t num_ctor_params , ParamHandle* params) {
        if (!entity->HasComponent<components::Script>())
            return;

        auto& script = entity->GetComponent<components::Script>();
        auto& eid = entity->GetComponent<components::ID>();

        if (!script.bound){
            YE_WARN("Attempting to initialize unbound script on entity :: [{0} , {1}]" , script.object->name , eid.name);
        }

        for (auto& field : script.object->fields) {
            ScriptField* field = ScriptMap::GetFieldByID(script.object , field->IDU32);
            if (field == nullptr) {
                YE_ERROR("Failed to find script field: {0} when trying to initialize: {1}" , field->name , script.object->name);
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
    }

    void ScriptEngine::ActivateEntity(Entity* entity) {
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
    }
    
    void ScriptEngine::DeactivateEntity(Entity* entity, bool erase) {
        // if (!entity.HasComponent<components::Script>())
        //     return;

        // auto& script = entity.GetComponent<components::Script>();
        // UUID eid = entity.GetComponent<components::ID>().id;

        // if (!script.bound || script.active)
        //     return;

        // if (erase && internal_state->context_entities->find(eid) != internal_state->context_entities.end())
        //     internal_state->context_entities->erase(eid);
    }
    
    void ScriptEngine::DestroyEntity(Entity* entity) {
        if (!entity->HasComponent<components::Script>())
            return;

        auto& script = entity->GetComponent<components::Script>();

        if (!script.bound || !script.active)
            return;

        InvokeDestroy(script.object , script.instance , script.handle);

        // do something with fields
        ScriptGC::FreeHandle(script.handle);
        script.handle = nullptr;
        script.bound = false;
        script.active = false;
    }

    void ScriptEngine::InvokeCreate(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
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
    }

    void ScriptEngine::InvokeUpdate(ScriptObject* obj , MonoObject* instance , GCHandle handle , float delta_time) {
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
    }

    void ScriptEngine::InvokeDestroy(ScriptObject* obj , MonoObject* instance , GCHandle handle) {
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
    }
    
    void ScriptEngine::InvokeMethod(MonoObject* obj , ScriptMethod* method , ParamHandle* params) {
        YE_CRITICAL_ASSERTION(obj != nullptr , "Attempting to call method on null object");
        YE_CRITICAL_ASSERTION(method != nullptr , "Attempting to call null method");
        YE_CRITICAL_ASSERTION(method->method != nullptr , "Attempting to call null method");

        if (method->param_count > 0)
            YE_CRITICAL_ASSERTION(params != nullptr , "Attempting to call method with null params"
        );

        MonoObject* exception = nullptr;
        mono_runtime_invoke(method->method , obj , params , &exception);
        CHECK_MONO_EXCEPTION(exception);
    }

    void ScriptEngine::Initialize() {
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
    }
    
    void ScriptEngine::LoadProjectModules() {
        YE_CRITICAL_ASSERTION(initialized , "Attempting to load project modules before initializing script engine");
        LoadProjectScripts();
        ScriptMap::LoadProjectTypes();
    }

    void ScriptEngine::UnloadProjectModules() {
        YE_CRITICAL_ASSERTION(initialized , "Attempting to unload project modules before initializing script engine");
        ScriptMap::UnloadProjectTypes();
        UnloadProjectScripts();
    }

    void ScriptEngine::ReloadProjectModules() {

        bool scene_check = scene_started;
        if (scene_check)
            StopScene();

        ScriptGC::Collect(true);
        UnloadProjectModules();
        ScriptGC::Shutdown();
        ScriptMap::Destroy();
        
        std::filesystem::path module_proj_path = Filesystem::GetProjectModulesPath();
        std::string module_path = Filesystem::GetModulePath();

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
        system(command.c_str());
#endif

        LoadInternalScripts();
        ScriptMap::Generate();
        ScriptGC::Initialize();

        LoadProjectModules();

        for (auto& [id , entity] : *internal_state->context_entities) {
            if (!entity->HasComponent<components::Script>())
                continue;
            auto& script = entity->GetComponent<components::Script>();
            script.Bind(script.class_name);
        }

        if (scene_check)
            StartScene();

        scripts_reloaded = true;
    }
    
    void ScriptEngine::SetSceneContext(Scene* scene) {
        YE_CRITICAL_ASSERTION(scene != nullptr , "Attempting to set null scene context");

        internal_state->scene_context = scene;
        internal_state->context_entities = scene->Entities();
    }
    
    void ScriptEngine::StartScene() {
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
    }

    void ScriptEngine::StopScene() {
        YE_CRITICAL_ASSERTION(initialized , "Attempting to stop scene before initializing script engine");
        YE_CRITICAL_ASSERTION(internal_state->scene_context != nullptr , "Attempting to stop scene with null scene context");

        for (auto& [id , entity] : *internal_state->context_entities) {
            if (!entity->HasComponent<components::Script>())
                continue;
            
            auto& script = entity->GetComponent<components::Script>();
            if (script.bound)
                script.Unbind();

            if (!script.active) {
                YE_WARN(
                    "Attempting to stop inactive script on entity :: [{0} , {1}]" , 
                    script.object->name , entity->GetComponent<components::ID>().name
                );
                continue;
            }

            DestroyEntity(entity);
        }

        scene_started = false;
    }
    
    void ScriptEngine::Shutdown() {
        ScriptGC::Shutdown();
        ScriptMap::Destroy();
        ScriptGlue::UnbindAssembly();
        ShutdownMono();
        initialized = false;
    }
    
    void ScriptEngine::Cleanup() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }
}