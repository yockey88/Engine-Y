#ifndef YE_SCRIPT_ENGINE_HPP
#define YE_SCRIPT_ENGINE_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <filesystem>

#include "mono/metadata/assembly.h"
#include "mono/jit/jit.h"

#include "script_glue.hpp"
#include "script_base.hpp"
#include "script_map.hpp"
#include "garbage_collector.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"

#ifdef YE_DEBUG_BUILD
    static const std::string kMonoAssembliesPath = "external/mono/lib/Debug";
#else
    static const std::string kMonoAssembliesPath = "external/mono/lib/Release";
#endif

// seemed cool before I started typing :'() 
// it does allow me to avoid copies though so I guess it's not that bad
// maps entity UUID -> map of their fields (UUID -> field)
typedef std::unordered_map<
    YE::UUID , 
    std::unordered_map<
        YE::UUID32 , std::unique_ptr<YE::FieldBase>
    >
> EntityFieldMap; 

namespace YE {

    class ScriptEngine {

        static ScriptEngine* instance;

        MonoDomain* root_domain = nullptr;
        MonoDomain* app_domain = nullptr;
        std::string root_domain_name;
        std::string app_domain_name;

        std::string mono_path;
        std::string mono_config_path;
        std::string internal_modules_path;
        std::string project_modules_path;
        
        bool initialized = false; 
        bool scripts_reloaded = false;
        bool scene_started = false;
        bool scene_context_exists = false;

        struct AssemblyProperties {
            std::string name = "";
            uint32_t version[4] = { 0 , 0 , 0 , 0 };
            std::vector<AssemblyProperties> referenced_assemblies{};
        };

        struct ScriptData {
            MonoImage* image = nullptr;
            MonoAssembly* assembly = nullptr;
        };

        struct EngineState {
            Scene* scene_context = nullptr;
            std::unordered_map<UUID , Entity*>* context_entities = nullptr;
            EntityFieldMap entity_field_map{};
            std::unordered_map<UUID , GCHandle> entity_handles{};
            bool reload = false;
        };

        AssemblyProperties core_asm_properties;
        AssemblyProperties project_asm_properties;

        ScriptData internal_script_data;
        ScriptData project_script_data;
        
        std::unique_ptr<EngineState> internal_state;
        
        void GetAssemblyProperties(MonoImage* img , AssemblyProperties& properties);
        void GetReferencedAssemblies(MonoImage* img , AssemblyProperties& properties);
        MonoAssembly* LoadCSharpAsm(MonoImage *& img , const std::string& filepath , bool internal = false);

        void InitializeScriptDebugging(uint16_t port);
        void CreateDebugDomains();
        void LoadDebugImage(MonoImage* img , const std::string& filepath);
        
        uint32_t BuildProjectModules(std::filesystem::path module_proj_path , std::string module_path);
        void LoadInternalScripts();
        void LoadProjectScripts();
        void UnloadProjectScripts();

        void GetField(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value);
        void GetProperty(ScriptObject* obj , ScriptField* field , MonoObject* instance , Field* value);

        void SetField(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value);
        void SetProperty(ScriptObject* obj , ScriptField* sf , MonoObject* instance , GCHandle handle , Field* value);

        void ShutdownMono(); 
        void PrintCoreAsmInfo();

        ScriptEngine() {}
        ~ScriptEngine() {}

        ScriptEngine(ScriptEngine&&) = delete;
        ScriptEngine(const ScriptEngine&) = delete;
        ScriptEngine& operator=(ScriptEngine&&) = delete;
        ScriptEngine& operator=(const ScriptEngine&) = delete;

        friend class ScriptMap;

        public:
            static ScriptEngine* Instance();

            inline MonoDomain* GetAppDomain() const { return app_domain; }
            
            ScriptObject* CreateObject(MonoObject *& instance , const std::string& name);
            void DestroyObject(ScriptObject* obj , MonoObject* instance , GCHandle handle);

            void InstantiateObject(ScriptObject* obj , MonoObject* instance); 

            void GetObjField(ScriptObject* obj , MonoObject* instance , const std::string& field , Field* value);
            void SetObjField(ScriptObject* obj , MonoObject* instance , GCHandle handle , const std::string& field , Field* value);

            void InitializeEntity(Entity* entity , uint32_t num_ctor_params , ParamHandle* params);
            void ActivateEntity(Entity* entity);
            void DeactivateEntity(Entity* entity, bool erase = true);
            void DestroyEntity(Entity* entity);

            void InvokeCreate(ScriptObject* obj , MonoObject* instance , GCHandle handle);
            void InvokeUpdate(ScriptObject* obj , MonoObject* instance , GCHandle handle , float delta_time);
            void InvokeDestroy(ScriptObject* obj , MonoObject* instance , GCHandle handle);
            void InvokeMethod(MonoObject* obj , ScriptMethod* method , ParamHandle* params);
            
            void Initialize();
            void LoadProjectModules();
            void UnloadProjectModules();

            void ReloadProjectModules();
            inline void AcknowledgeModuleReload() { scripts_reloaded = false; }
            inline const bool ModulesReloaded() const { return scripts_reloaded; }
            inline const bool SceneStarted() const { return scene_started; }

            void Shutdown();
            void Cleanup();

            void SetSceneContext(Scene* scene);
            void StartScene();
            void StopScene();

            inline ScriptData* GetInternalScriptData() { return &internal_script_data; }
            inline ScriptData* GetProjectScriptData() { return &project_script_data; }
            
            inline Scene* GetSceneContext() { return internal_state->scene_context; }
            inline bool IsInitialized() const { return initialized; }
    };

}

#endif // !YE_SCRIPT_ENGINE_HPP