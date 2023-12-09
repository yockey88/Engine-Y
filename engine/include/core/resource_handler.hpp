#ifndef ENGINEY_RESOURCE_HANDLER_HPP
#define ENGINEY_RESOURCE_HANDLER_HPP

#include <string>
#include <unordered_map>

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdfgen/msdfgen.h>

#include "core/UUID.hpp"
#include "core/log.hpp"
#include "core/reference.hpp"
#include "rendering/model.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/vertex_array.hpp"

///> Todo:
///    - Add lazy loading for the rest of the resources
///    - convert models to Ref<Model> instead of Model*


struct ImFont;

namespace EngineY {

    /// temporary until find better palce to put this 
    struct FontResource {
        std::string name;
        std::string path;
        ImFont* font;
    };

    enum class ResourceType {
        CORE , APP
    };    
    
    template<typename T>
    using ResourceMap = std::unordered_map<UUID , T>;

    class ResourceHandler {

        static ResourceHandler* singleton;

        msdfgen::FreetypeHandle* freetype = nullptr;

        ResourceMap<FontResource> engine_fonts;


        ///> Having seperator caches for app/engine shaders allows us to not deal
        ///    with a user creating a shader with the same name as a core shader
        ///    (there are better ways to do this but this is the easiest, will revisit)
        ResourceMap<ShaderResource> engine_shaders;
        std::map<UUID , Ref<Shader>> cached_core_shaders;

        ResourceMap<ShaderResource> app_shaders;
        std::map<UUID , Ref<Shader>> cached_shaders;

        ResourceMap<TextureResource> engine_textures;
        std::map<UUID , Ref<Texture>> cached_core_textures;

        ResourceMap<TextureResource> app_textures;
        std::map<UUID , Ref<Texture>> cached_textures;

        std::map<UUID , Ref<VertexArray>> cached_vaos;
        ResourceMap<VertexArrayResource> primitive_vaos;

        ResourceMap<ModelResource> engine_models;
        ResourceMap<ModelResource> app_models;

        std::string engine_resource_dir;
        std::string engine_shader_dir;
        std::string engine_texture_dir;
        std::string engine_model_dir;

        std::string app_resource_dir;
        std::string app_shader_dir;
        std::string app_texture_dir;
        std::string app_model_dir;

        bool shaders_reloaded = false;

        bool StoreShader(
            const std::string& dir_path , const std::string& name , UUID id , 
            std::map<UUID , Ref<Shader>>& shader_refs , 
            ResourceMap<ShaderResource>& shader_map
        );
        bool ReloadShader(
            UUID id , Ref<Shader>& shader , 
            std::map<UUID , Ref<Shader>>& shader_refs , 
            ResourceMap<ShaderResource>& shader_map
        );
        bool StoreTexture(
            const std::string& dir_path , const std::string& name , UUID id , 
            std::map<UUID , Ref<Texture>>& shader_refs , ResourceMap<TextureResource>& textures
        );
        void StoreFonts(const std::string& dir_path , ResourceMap<FontResource>& fonts);
        void GeneratePrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos);
        void StoreModels(const std::string& dir_path , ResourceMap<ModelResource>& models);

        void CleanupShaders(std::map<UUID , Ref<Shader>>& shader_refs , ResourceMap<ShaderResource>& shaders);
        void CleanupTextures(std::map<UUID , Ref<Texture>>& texture_refs , ResourceMap<TextureResource>& textures);
        void CleanupFonts(ResourceMap<FontResource>& fonts);
        void CleanupPrimitiveVAOs(std::map<UUID , Ref<VertexArray>>& vao_refs , ResourceMap<VertexArrayResource>& vaos);
        void CleanupModels(ResourceMap<ModelResource>& models);

        ResourceHandler() {}
        ~ResourceHandler() {}

        template<typename T>
        bool CheckID(UUID id , const std::string& name , const ResourceMap<T>& map) {
            if (map.find(id) != map.end()) {
                ENGINE_WARN("Failed to register resource :: [{0}] | Name already exists" , name);
                return false;
            }
            return true;
        }

        ResourceHandler(ResourceHandler&&) = delete;
        ResourceHandler(const ResourceHandler&) = delete;
        ResourceHandler& operator=(ResourceHandler&&) = delete;
        ResourceHandler& operator=(const ResourceHandler&) = delete;

        public:

            static ResourceHandler* Instance();

            void Load();
            void Offload();
           
            ///> \todo implement lazy loading for the rest of these
            ///    and make these all references and not pointers
            ///    to avoid the need to track with mem manager
            
            Ref<Shader> GetCoreShader(const std::string& name);
            Ref<Shader> GetShader(const std::string& name);

            Ref<Texture> GetCoreTexture(const std::string& name);
            Ref<Texture> GetTexture(const std::string& name);

            ImFont* GetCoreFont(const std::string& name);

            ///> Lazy loaded implemented here
            Ref<VertexArray> GetPrimitiveVAO(const std::string& name);

            Model* GetCoreModel(const std::string& name);
            Model* GetModel(const std::string& name);

            void ReloadShaders();

            void Cleanup();

            inline void AcknowledgeShaderReload() { shaders_reloaded = false; }
            inline bool ShadersReloaded() const { return shaders_reloaded; }
            inline msdfgen::FreetypeHandle* Freetype() const { return freetype; }
    };

}


#endif // !YE_RESOURCE_HANDLER_HPP
