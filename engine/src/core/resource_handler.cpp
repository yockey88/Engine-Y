#include "core/resource_handler.hpp"

#include <array>
#include <algorithm>

#include <stb_image.h>
#include <imgui/imgui.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdfgen/ext/import-font.h>

#include "core/log.hpp"
#include "core/filesystem.hpp"
#include "core/task_manager.hpp"
#include "core/memory/memory_manager.hpp"
#include "rendering/primitive_vao_data.hpp"

namespace EngineY {

    ResourceHandler* ResourceHandler::singleton = nullptr;
    
    void ResourceHandler::StoreShaders(const std::string& dir_path , ResourceMap<ShaderResource>& shaders) {
        std::filesystem::path path = dir_path;
        if (!std::filesystem::exists(path)) {
            ENGINE_WARN("Failed to load shaders :: [{0}] | Directory does not exist" , dir_path);
            return;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() != ".vert" && entry.path().extension() != ".frag" &&
                entry.path().extension() != ".geom") continue;
            
            UUID id = Hash::FNV(entry.path().stem().string());
            if (shaders.find(id) == shaders.end()) {
                ShaderResource shader;
                shader.name = entry.path().stem().string();
                if (entry.path().extension() == ".vert") {
                    shader.vert_filename = entry.path().filename().string();
                    shader.vert_path = entry.path().string();
                    std::replace(shader.vert_path.begin() , shader.vert_path.end() , '\\' , '/');
                } else if (entry.path().extension() == ".frag") {
                    shader.frag_filename = entry.path().filename().string();
                    shader.frag_path = entry.path().string();
                    std::replace(shader.frag_path.begin() , shader.frag_path.end() , '\\' , '/');
                } else if (entry.path().extension() == ".geom") {
                    shader.geom_filename = entry.path().filename().string();
                    shader.geom_path = entry.path().string();
                    std::replace(shader.geom_path.begin() , shader.geom_path.end() , '\\' , '/');
                    shader.has_geom = true;
                }
                shaders[id] = shader;
            } else {
                ShaderResource& shader = shaders[id];
                if (entry.path().extension() == ".vert") {
                    shader.vert_filename = entry.path().filename().string();
                    shader.vert_path = entry.path().string();
                    std::replace(shader.vert_path.begin() , shader.vert_path.end() , '\\' , '/');
                } else if (entry.path().extension() == ".frag") {
                    shader.frag_filename = entry.path().filename().string();
                    shader.frag_path = entry.path().string();
                    std::replace(shader.frag_path.begin() , shader.frag_path.end() , '\\' , '/');
                } else if (entry.path().extension() == ".geom") {
                    shader.geom_filename = entry.path().filename().string();
                    shader.geom_path = entry.path().string();
                    std::replace(shader.geom_path.begin() , shader.geom_path.end() , '\\' , '/');
                    shader.has_geom = true;
                }
            }
        }
    }

    void ResourceHandler::StoreTextures(const std::string& dir_path , ResourceMap<TextureResource>& textures) {
        std::filesystem::path path = dir_path;
        if (!std::filesystem::exists(path)) {
            ENGINE_WARN("Failed to load textures :: [{0}] | Directory does not exist" , dir_path);
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() != ".png" && entry.path().extension() != ".jpg" &&
                entry.path().extension() != ".jpeg") continue;

            UUID id = Hash::FNV(entry.path().filename().string());
            TextureResource texture;
            texture.name = entry.path().stem().string();
            texture.filename = entry.path().filename().string();
            texture.path = entry.path().string();

            if (entry.path().extension() == ".png") {
                texture.channels = ChannelType::RGBA;
            } else if (entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg") {
                texture.channels = ChannelType::RGB;
            }

            texture.target = TargetType::TEX_2D;
            std::replace(texture.path.begin() , texture.path.end() , '\\' , '/');
            textures[id] = texture;
        }
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

    // quad , cube , sphere , icosahedron , icosphere , cubesphere , cylinder , cone , torus
    void ResourceHandler::GeneratePrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos) {
        VertexArrayResource quad_vao;
        quad_vao.name = "quad";
        quad_vao.vao = ynew(VertexArray , primitives::quad_verts , primitives::quad_indices);
        vaos[Hash::FNV(quad_vao.name)] = quad_vao;

        VertexArrayResource cube_vao;
        cube_vao.name = "cube";
        cube_vao.vao = ynew(VertexArray , primitives::cube_verts , std::vector<uint32_t>{});
        vaos[Hash::FNV(cube_vao.name)] = cube_vao;

        VertexArrayResource icosadren_vao;
        icosadren_vao.name = "icosahedron";
        std::vector<float> icosahedron_verts = primitives::VertexBuilder::BuildIcosahedronVerts();
        std::vector<uint32_t> icosahedron_indices = primitives::VertexBuilder::GetIcosahedronIndices();
        icosadren_vao.vao = ynew(VertexArray , icosahedron_verts , icosahedron_indices , { 3 });
        vaos[Hash::FNV(icosadren_vao.name)] = icosadren_vao;
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

    void ResourceHandler::CompileShaders(ResourceMap<ShaderResource>& shaders) {
        for (auto& [id , shader] : shaders) {
            Shader* s = nullptr;
            if (shader.has_geom) {
                s = ynew(Shader , shader.vert_path , shader.frag_path , shader.geom_path);
            } else {
                s = ynew(Shader , shader.vert_path , shader.frag_path);
            }
            if (!s->Compile()) {
                ENGINE_WARN("Failed to compile shader [{0}] :: [{1}] | [{2}] | [{3}]" , shader.name , shader.vert_path , shader.frag_path ,
                                                                                    shader.has_geom ? shader.geom_path : "<no geometry>");
                ydelete(s);
                s = nullptr;
            } else {
                s->SetName(shader.name);
            }
            shader.shader = s;
        }
    }

    void ResourceHandler::LoadTextures(ResourceMap<TextureResource>& textures) {
        for (auto& [id , texture] : textures) {
            Texture* t = ynew(Texture , texture.path);
            t->Load(texture.target , texture.channels);
            t->SetName(texture.name);
            texture.texture = t;
        }
    }

    void ResourceHandler::LoadModels(ResourceMap<ModelResource>& models) {
        for (auto& [id , model] : models) {
            Model* m = ynew(Model , model.name , model.path);
            m->Load();
            model.model = m;
        }
    }
    
    void ResourceHandler::CleanupShaders(ResourceMap<ShaderResource>& shaders) {
        for (auto& [id , s] : shaders) {
            if (s.shader != nullptr) {
                ydelete(s.shader);
            }
        }
        shaders.clear();
    }

    void ResourceHandler::CleanupTextures(ResourceMap<TextureResource>& textures) {
        for (auto& [id , t] : textures) {
            if (t.texture != nullptr) {
                ydelete(t.texture); 
            }
        }
        textures.clear();
    }
    
    void ResourceHandler::CleanupFonts(ResourceMap<FontResource>& fonts) {
        /// imgui deletes the fonts itself
        fonts.clear();
    }

    void ResourceHandler::CleanupPrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos) {
        for (auto& [id , vao] : vaos) {
            if (vao.vao != nullptr) {
                ydelete(vao.vao);
            }
        }
        vaos.clear();
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

        StoreShaders(engine_shader_dir , engine_shaders);
        StoreShaders(app_shader_dir , app_shaders);
        CompileShaders(engine_shaders);
        CompileShaders(app_shaders);

        StoreTextures(engine_texture_dir , engine_textures);
        StoreTextures(app_texture_dir , app_textures);
        LoadTextures(engine_textures);
        LoadTextures(app_textures);

        StoreFonts(font_dir , engine_fonts);

        GeneratePrimitiveVAOs(primitive_vaos);

        StoreModels(engine_model_dir , engine_models);
        StoreModels(app_model_dir , app_models);
        LoadModels(engine_models);
        LoadModels(app_models);
    }

    void ResourceHandler::Offload() {
        CleanupShaders(engine_shaders);
        CleanupShaders(app_shaders);
        CleanupTextures(engine_textures);
        CleanupTextures(app_textures);
        CleanupFonts(engine_fonts);
        CleanupPrimitiveVAOs(primitive_vaos);
        CleanupModels(engine_models);
        CleanupModels(app_models);
        
        msdfgen::deinitializeFreetype(freetype);
    }

    void ResourceHandler::AddShader(
        const std::string& vert_path , const std::string& frag_path ,
        const std::string& geom_path , ResourceType type
    ) {
        // UUID id = Hash::FNV(vert_path + frag_path + geom_path);
        // if (!CheckID(id , vert_path , shaders)) return;
        // shaders[id].shader = ynew Shader(vert_path , frag_path , geom_path);
        // if (!shaders[id].shader->Compile()) {
        //     ENGINE_WARN("Failed to compile shader :: [{0}] | [{1}] | [{2}]" , vert_path , frag_path , geom_path);
        //     ydelete shader;
        //     return;
        // }
    }

    void ResourceHandler::AddTexture(const std::string& path , ResourceType type) {
        // UUID id = Hash::FNV(path);
        // if (!CheckID(id , path , textures)) return;
        // textures[id].texture = ynew Texture(path);
        // textures[id].texture->Load();
    }
    
    void ResourceHandler::AddTexture(const std::string& name , Texture* texture , ResourceType type) {
        UUID id = Hash::FNV(name);
        switch (type) {
            case ResourceType::CORE: {
                if (!CheckID(id , name , engine_textures)) return;
                engine_textures[id].texture = texture;
                break;
            } 
            case ResourceType::APP: {
                if (!CheckID(id , name , app_textures)) return;
                app_textures[id].texture = texture;
                break;
            }
            default:
                ENGINE_WARN("Failed to add texture :: [{0}] | Invalid resource type" , name);
                break;
        }
    } 

    void ResourceHandler::AddModel(const std::string& path , ResourceType type) {
        // UUID id = Hash::FNV(path);
        // if (!CheckID(id , path , models)) return;
        // models[id].model = ynew Model(path);
        // models[id].model->Load();
    }

    Shader* ResourceHandler::GetCoreShader(const std::string& name) {
        for (auto& [id , shader] : engine_shaders)
            if (shader.name == name) return shader.shader;
        return nullptr;
    }

    Shader* ResourceHandler::GetShader(const std::string& name) {
        for (auto& [id , shader] : app_shaders)
            if (shader.name == name) return shader.shader;
        
        return nullptr;
    }
    
    Texture* ResourceHandler::GetCoreTexture(const std::string& name) {
        for (auto& [id , texture] : engine_textures)
            if (texture.name == name) return texture.texture;
        return nullptr;
    }

    Texture* ResourceHandler::GetTexture(const std::string& name) {
        for (auto& [id , texture] : app_textures)
            if (texture.name == name) return texture.texture;
        return nullptr;
    }
    
    ImFont* ResourceHandler::GetCoreFont(const std::string& name) {
        for (auto& [id , font] : engine_fonts)
            if (font.name == name) return font.font;
        return nullptr;
    }

    Ref<VertexArray> ResourceHandler::GetPrimitiveVAO(const std::string& name) {
        UUID id = Hash::FNV(name);
        auto itr = cached_vaos.find(id);

        if (itr != cached_vaos.end()) {
            return itr->second;
        } else {
            auto end = primitives::VertexBuilder::primitive_ids.end();
            auto it = std::find(primitives::VertexBuilder::primitive_ids.begin() , end , id);
            if (it != end) {
                primitives::VaoData data = primitives::VertexBuilder::GetPrimitiveData(id);
                ENGINE_ASSERT(
                    data.first.size() != 0 && data.second.size() != 0 ,
                    "Failed to get primitive data :: [{0}]" , name
                );

                ///> Need to fix how im handling layouts here, probably should buffer pos, normal, tangent, bitangents, and uvs,
                ///     and add in a different system for custom layouts?
                Ref<VertexArray> vao = Ref<VertexArray>::Create(data.first , data.second , std::vector<uint32_t>{ 3 });
                cached_vaos[id] = vao;

                return vao;
            } else {
                ENGINE_INFO("Failed to get primitive data (did you spell the name right?) :: [{0}]" , name);
                return nullptr;
            }
        }

        ENGINE_ASSERT(false , "UNREACHABLE");
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
        CleanupShaders(engine_shaders);
        CleanupShaders(app_shaders);
        StoreShaders(engine_shader_dir , engine_shaders);
        StoreShaders(app_shader_dir , app_shaders);
        CompileShaders(engine_shaders);
        CompileShaders(app_shaders);
        shaders_reloaded = true;
    }

    void ResourceHandler::Cleanup() {
        if (singleton != nullptr) {
            delete singleton;
        }
    }

}
