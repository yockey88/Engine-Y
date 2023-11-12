#ifndef YE_RESOURCE_HANDLER_HPP
#define YE_RESOURCE_HANDLER_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "log.hpp"
#include "UUID.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/model.hpp"

struct ImFont;

namespace YE {

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

        ResourceMap<ShaderResource> engine_shaders;
        ResourceMap<TextureResource> engine_textures;
        ResourceMap<FontResource> engine_fonts;

        ResourceMap<ShaderResource> app_shaders;
        ResourceMap<TextureResource> app_textures;

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

        void StoreShaders(const std::string& dir_path , ResourceMap<ShaderResource>& shaders);
        void StoreTextures(const std::string& dir_path , ResourceMap<TextureResource>& textures);
        void StoreFonts(const std::string& dir_path , ResourceMap<FontResource>& fonts);
        void GeneratePrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos);
        void StoreModels(const std::string& dir_path , ResourceMap<ModelResource>& models);

        void CompileShaders(ResourceMap<ShaderResource>& shaders);
        void LoadTextures(ResourceMap<TextureResource>& textures);
        void UploadPrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos);
        void LoadModels(ResourceMap<ModelResource>& models);

        void CleanupShaders(ResourceMap<ShaderResource>& shaders);
        void CleanupTextures(ResourceMap<TextureResource>& textures);
        void CleanupFonts(ResourceMap<FontResource>& fonts);
        void CleanupPrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos);
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

            void AddShader(
                const std::string& vert_path , const std::string& frag_path ,
                const std::string& geom_path = "" , ResourceType type = ResourceType::CORE
            );
            void AddShader(const std::string& name , Shader* shader , ResourceType type = ResourceType::CORE) {}
            void AddTexture(const std::string& path , ResourceType type = ResourceType::CORE);
            void AddTexture(const std::string& name , Texture* texture , ResourceType type = ResourceType::CORE); 
            void AddModel(const std::string& path , ResourceType type = ResourceType::CORE);
            void AddModel(const std::string& name , Model* model , ResourceType type = ResourceType::CORE) {}
            
            Shader* GetCoreShader(const std::string& name);
            Shader* GetShader(const std::string& name);

            Texture* GetCoreTexture(const std::string& name);
            Texture* GetTexture(const std::string& name);

            ImFont* GetCoreFont(const std::string& name);

            VertexArray* GetPrimitiveVAO(const std::string& name);

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