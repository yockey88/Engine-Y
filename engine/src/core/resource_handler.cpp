#include "core/resource_handler.hpp"

#include <array>
#include <algorithm>

#include <stb_image.h>
#include <imgui/imgui.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdfgen/ext/import-font.h>

#include "core/log.hpp"
#include "core/filesystem.hpp"
#include "rendering/primitive_vao_data.hpp"

namespace EngineY {

    ResourceHandler* ResourceHandler::singleton = nullptr;
    
    bool ResourceHandler::StoreShader(
        const std::string& dir_path , const std::string& name ,  UUID id ,
        std::map<UUID , Ref<Shader>>& shader_refs , ResourceMap<ShaderResource>& shader_map
    ) {
        ENGINE_FDEBUG("Generating and caching shader :: [{0}]" , name);
        std::string vert_path = dir_path + "/" + name + ".vert";
        std::string frag_path = dir_path + "/" + name + ".frag";

        if (!Filesystem::FileExists(vert_path) || !Filesystem::FileExists(frag_path)) {
            ENGINE_ERROR("Failed to get shader :: [{0}] | Shaders require both a vertex and fragment shader" , name);
            return false;
        }

        ShaderResource shader;
        shader.name = name;

        std::string geom_path = dir_path + "/" + name + ".geom";
        shader.has_geom = Filesystem::FileExists(geom_path);

        if (shader.has_geom) {
            shader.geom_filename = name + ".geom";
            shader.geom_path = geom_path;
        } else {
            shader.geom_filename = "";
            shader.geom_path = "";
        }

        shader.vert_filename = name + ".vert";
        shader.frag_filename = name + ".frag";
        shader.vert_path = vert_path;
        shader.frag_path = frag_path;

        shader.shader = shader.has_geom ? 
            Ref<Shader>::Create(shader.vert_path , shader.frag_path , shader.geom_path) :
            Ref<Shader>::Create(shader.vert_path , shader.frag_path);

        if (!shader.shader->Compile()) {
            std::string error_msg = fmt::format(fmt::runtime(
                "Failed to compile shader :: [{0}] | [{1}] | [{2}]") , 
                name , shader.vert_path , shader.frag_path
            );

            if (shader.has_geom) {
                error_msg += fmt::format(fmt::runtime(" | [{0}]") , shader.geom_path);
            }
        
            ENGINE_ERROR(error_msg);
            return false;
        }

        shader.shader->SetName(name);

        shader_map[id] = shader;
        shader_refs[id] = Ref<Shader>::WriteTo(shader.shader);

        return true;
    }
    
    bool ResourceHandler::ReloadShader(
        UUID id , Ref<Shader>& shader , 
        std::map<UUID , Ref<Shader>>& shader_refs , 
        ResourceMap<ShaderResource>& shader_map
    ) {
        shader->MarkForReload();

        std::string name = shader->Name();
        std::string vert_path = shader->VertexPath();
        std::string frag_path = shader->FragmentPath();
        std::string geom_path = shader->HasGeometry() ?
            shader->GeometryPath() : "";

        ENGINE_FDEBUG("Reloading shader :: [{0}]" , name);

        shader = shader->HasGeometry() ? 
            Ref<Shader>::Create(vert_path , frag_path , geom_path) :
            Ref<Shader>::Create(vert_path , frag_path);

        shader->SetName(name);

        shader_map[id].shader = shader;
        shader_refs[id] = Ref<Shader>::WriteTo(shader);

        return shader->Compile();
    }

    bool ResourceHandler::StoreTexture(
        const std::string& dir_path , const std::string& name , UUID id , 
        std::map<UUID , Ref<Texture>>& shader_refs , ResourceMap<TextureResource>& textures
    ) {
        ENGINE_FDEBUG("Loading and caching texture :: [{0}]" , name);
        std::string filename = name + ".png";
        std::string path = dir_path + "/" + filename;

        if (!Filesystem::FileExists(path)) {
            ENGINE_FINFO("Failed to load texture as png :: [{0}] | checking for for .jpg extension" , name);
            filename = name + ".jpg";
            path = dir_path + "/" + filename;
            if (!Filesystem::FileExists(path)) {
                ENGINE_ERROR("Failed to load texture :: [{0}] | File does not exist" , name);
                return false;
            }
        }

        TextureResource texture;
        texture.name = name;
        texture.filename = filename;
        texture.path = path;
        texture.texture = Ref<Texture>::Create(path);

        texture.texture->Load();

        textures[id] = texture;
        shader_refs[id] = Ref<Texture>::WriteTo(texture.texture);

        return true;
    }
    
    void ResourceHandler::StoreFonts(const std::string& dir_path , ResourceMap<FontResource>& fonts) {
        std::string res = Filesystem::GetEngineResPath();
        
        ENGINE_INFO("Loading fonts from :: [{0}]" , res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf");

        FontResource blex_mono {
            "BlexMono" , 
            res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf" , 
            ImGui::GetIO().Fonts->AddFontFromFileTTF((res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf").c_str() , 18.f)
        };

        fonts[Hash::FNV(blex_mono.name)] = blex_mono;
    }

    void ResourceHandler::StoreModels(const std::string& dir_path , ResourceMap<ModelResource>& models) {
        // std::filesystem::path path = dir_path;
        // if (!std::filesystem::exists(path)) {
        //     ENGINE_WARN("Failed to load models :: [{0}] | Directory does not exist" , dir_path);
        //     return;
        // }
        
        // for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        //     if (entry.path().extension() != ".obj") continue;

        //     UUID id = Hash::FNV(entry.path().filename().string());
        //     ModelResource model;
        //     model.name = entry.path().stem().string();
        //     model.filename = entry.path().filename().string();
        //     model.path = entry.path().string();
        //     std::replace(model.path.begin() , model.path.end() , '\\' , '/');
        //     models[id] = model;
        // }
    }

    void ResourceHandler::CleanupShaders(std::map<UUID , Ref<Shader>>& shader_refs , ResourceMap<ShaderResource>& shaders) {
        shaders.clear();
        shader_refs.clear();
    }

    void ResourceHandler::CleanupTextures(std::map<UUID , Ref<Texture>>& texture_refs , ResourceMap<TextureResource>& textures) {
        textures.clear();
        texture_refs.clear();
    }
    
    void ResourceHandler::CleanupFonts(ResourceMap<FontResource>& fonts) {
        /// imgui deletes the fonts itself
        fonts.clear();
    }

    void ResourceHandler::CleanupPrimitiveVAOs(std::map<UUID , Ref<VertexArray>>& vao_refs , ResourceMap<VertexArrayResource>& vaos) {
        vaos.clear();
        vao_refs.clear();
    }

    void ResourceHandler::CleanupModels(ResourceMap<ModelResource>& models) {
        for (auto& [id , m] : models)
            if (m.model != nullptr) {
                ydelete(m.model);
            }
        models.clear();
    }

    ResourceHandler* ResourceHandler::Instance() {
        if (singleton == nullptr) {
            singleton = new ResourceHandler;
        }
        return singleton;
    }

    void ResourceHandler::Load() {
        // how to determine which textures need this and which dont?
        // stbi_set_flip_vertically_on_load(true);
        freetype = msdfgen::initializeFreetype();

        engine_resource_dir = Filesystem::GetEngineResPath();
        engine_shader_dir = Filesystem::GetEngineShaderPath();
        engine_texture_dir = Filesystem::GetEngineTexturePath();
        engine_model_dir = Filesystem::GetEngineModelPath();

        app_resource_dir = Filesystem::GetResPath();
        app_shader_dir = Filesystem::GetShaderPath();
        app_texture_dir = Filesystem::GetTexturePath();
        app_model_dir = Filesystem::GetModelPath();
        
        std::string font_dir = engine_resource_dir + "/fonts";

        StoreFonts(font_dir , engine_fonts);
    }

    void ResourceHandler::Offload() {
        CleanupShaders(cached_core_shaders , engine_shaders);
        CleanupShaders(cached_shaders , app_shaders);
        CleanupTextures(cached_core_textures , engine_textures);
        CleanupTextures(cached_textures , app_textures);
        CleanupFonts(engine_fonts);
        CleanupPrimitiveVAOs(cached_vaos , primitive_vaos);
        CleanupModels(engine_models);
        CleanupModels(app_models);
        
        msdfgen::deinitializeFreetype(freetype);
    }

    Ref<Shader> ResourceHandler::GetCoreShader(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_core_shaders.find(id);

        if (itr != cached_core_shaders.end()) {
            ENGINE_FDEBUG("Retrieved cached core shader :: [{0}]" , name);
            return itr->second;
        } else {
            if (!StoreShader(engine_shader_dir , name , id , cached_core_shaders , engine_shaders)) {
                ENGINE_ERROR("Failed to compile and cache core shader :: [{0}]" , name);
                return nullptr;
            }

            return cached_core_shaders[id];
        }
    }

    Ref<Shader> ResourceHandler::GetShader(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_shaders.find(id);

        if (itr != cached_shaders.end()) {
            ENGINE_FDEBUG("Retrieved cached shader :: [{0}]" , name);
            return itr->second;
        } else {
            if (!StoreShader(app_shader_dir , name , id , cached_shaders , app_shaders)) {
                ENGINE_ERROR("Failed to compile and cache shader :: [{0}]" , name);
                return nullptr;
            }

            return cached_shaders[id]; 
        }
    }
    
    Ref<Texture> ResourceHandler::GetCoreTexture(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_core_textures.find(id);

        if (itr != cached_core_textures.end()) {
            ENGINE_FDEBUG("Retrieved cached core texture :: [{0}]" , name);
            return itr->second;
        } else {
            if (!StoreTexture(engine_texture_dir , name , id , cached_core_textures , engine_textures)) {
                ENGINE_ERROR("Failed to load and cache core texture :: [{0}]" , name);
                return nullptr;
            }

            return cached_core_textures[id];
        }
    }

    Ref<Texture> ResourceHandler::GetTexture(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_textures.find(id);

        if (itr != cached_textures.end()) {
            ENGINE_FDEBUG("Retrieved cached texture :: [{0}]" , name);
            return itr->second;
        } else {
            if (!StoreTexture(app_texture_dir , name , id , cached_textures , app_textures)) {
                ENGINE_ERROR("Failed to load and cache texture :: [{0}]" , name);
                return nullptr;
            }

            return cached_textures[id];
        }
    }
    
    ImFont* ResourceHandler::GetCoreFont(const std::string& name) {
        for (auto& [id , font] : engine_fonts)
            if (font.name == name) return font.font;
        return nullptr;
    }

    // quad , cube , sphere , icosahedron , icosphere , cubesphere , cylinder , cone , torus
    Ref<VertexArray> ResourceHandler::GetPrimitiveVAO(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_vaos.find(id);

        if (itr != cached_vaos.end()) {
            ENGINE_FDEBUG("Retrieved cached vao :: [{0}]" , name);
            return itr->second;
        } else {
            ENGINE_FDEBUG("Generating and caching vao :: [{0}]" , name);
            auto end = primitives::VertexBuilder::primitive_ids.end();
            auto it = std::find(primitives::VertexBuilder::primitive_ids.begin() , end , id);
            if (it != end) {
                primitives::VaoData data = primitives::VertexBuilder::GetPrimitiveData(id);
                ENGINE_ASSERT(data.second.size() != 0 && data.first.size() != 0 , "Failed to get primitive data");
                if (data.first.size() == 0 || data.second.size() == 0) {
                    ENGINE_FERROR(
                        "Failed to get primitive data :: [{0}]"
                        " | Vertices :: [{1}] ,  Indices :: [{2}]" , 
                        name , data.first.size() , data.first.size()
                    );
                    return nullptr;
                }

                ///> Need to fix how im handling layouts here, probably should buffer pos, normal, tangent, bitangents, and uvs,
                ///     and add in a different system for custom layouts?
                Ref<VertexArray> vao = Ref<VertexArray>::Create(
                    data.first , data.second ,
                    std::vector<uint32_t>{ 3 , 2 }
                );

                cached_vaos[id] = vao;
                return vao;
            } else {
                ENGINE_INFO("Failed to get primitive data (did you spell the name right?) :: [{0}]" , name);
                return nullptr;
            }
        }

        ENGINE_ASSERT(false , "UNREACHABLE CODE | UNDEFINED BEHAVIOR");
        return nullptr;
    }

    Model* ResourceHandler::GetCoreModel(const std::string& name) {
        for (auto& [id , model] : engine_models)
            if (model.name == name) return model.model;
        return nullptr;
    }

    Model* ResourceHandler::GetModel(const std::string& name) {
        for (auto& [id , model] : app_models)
            if (model.name == name) return model.model;
        return nullptr;
    }

    void ResourceHandler::ReloadShaders() {
        ENGINE_DEBUG("Reloading shaders");
        for (auto& [id , shader] : cached_core_shaders) {
            ReloadShader(id , shader , cached_core_shaders , engine_shaders);
        }

        for (auto& [id , shader] : cached_shaders) {
            ReloadShader(id , shader , cached_shaders , app_shaders);
        }

        shaders_reloaded = true;
    }

    void ResourceHandler::Cleanup() {
        if (singleton != nullptr) {
            delete singleton;
        }
    }

}
