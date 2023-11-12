#include "core/resource_handler.hpp"

#include <stb_image.h>
#include <imgui/imgui.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdfgen/ext/import-font.h>

#include "log.hpp"
#include "core/filesystem.hpp"
#include "core/task_manager.hpp"
#include "core/primitive_vao_data.hpp"

namespace YE {

    ResourceHandler* ResourceHandler::singleton = nullptr;
    
    void ResourceHandler::StoreShaders(const std::string& dir_path , ResourceMap<ShaderResource>& shaders) {
        ENTER_FUNCTION_TRACE_MSG(dir_path);

        std::filesystem::path path = dir_path;
        if (!std::filesystem::exists(path)) {
            ENGINE_WARN("Failed to load shaders :: [{0}] | Directory does not exist" , dir_path);
            EXIT_FUNCTION_TRACE();
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

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::StoreTextures(const std::string& dir_path , ResourceMap<TextureResource>& textures) {
        ENTER_FUNCTION_TRACE_MSG(dir_path);

        std::filesystem::path path = dir_path;
        if (!std::filesystem::exists(path)) {
            ENGINE_WARN("Failed to load textures :: [{0}] | Directory does not exist" , dir_path);
            EXIT_FUNCTION_TRACE();
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

        EXIT_FUNCTION_TRACE();
    }
    
    void ResourceHandler::StoreFonts(const std::string& dir_path , ResourceMap<FontResource>& fonts) {
        std::string res = Filesystem::GetEngineResPath();
        
        ENGINE_INFO("Loading fonts from :: [{0}]" , res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf");

        FontResource blex_mono {
            "BlexMono" , 
            res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf" , 
            ImGui::GetIO().Fonts->AddFontFromFileTTF((res + "/fonts/IBMPlexMono/BlexMonoNerdFontMono-Regular.ttf").c_str() , 16.f)
        };

        fonts[Hash::FNV(blex_mono.name)] = blex_mono;
    }

    // quad , cube , sphere , icosphere , cylinder , cone , torus , plane
    void ResourceHandler::GeneratePrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos) {
        ENTER_FUNCTION_TRACE();

        VertexArrayResource quad_vao;
        quad_vao.name = "quad";
        quad_vao.vao = ynew VertexArray(primitives::quad_verts , primitives::quad_indices);
        vaos[Hash::FNV(quad_vao.name)] = quad_vao;

        VertexArrayResource cube_vao;
        cube_vao.name = "cube";
        cube_vao.vao = ynew VertexArray(primitives::cube_verts , {});
        vaos[Hash::FNV(cube_vao.name)] = cube_vao;

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::StoreModels(const std::string& dir_path , ResourceMap<ModelResource>& models) {
        ENTER_FUNCTION_TRACE_MSG(dir_path);

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

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::CompileShaders(ResourceMap<ShaderResource>& shaders) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , shader] : shaders) {
            Shader* s = nullptr;
            if (shader.has_geom) {
                s = ynew Shader(shader.vert_path , shader.frag_path , shader.geom_path);
            } else {
                s = ynew Shader(shader.vert_path , shader.frag_path);
            }
            if (!s->Compile()) {
                ENGINE_WARN("Failed to compile shader [{0}] :: [{1}] | [{2}] | [{3}]" , shader.name , shader.vert_path , shader.frag_path ,
                                                                                    shader.has_geom ? shader.geom_path : "<no geometry>");
                ydelete s;
                s = nullptr;
            } else {
                s->SetName(shader.name);
            }
            shader.shader = s;
        }

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::LoadTextures(ResourceMap<TextureResource>& textures) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , texture] : textures) {
            Texture* t = ynew Texture(texture.path);
            t->Load(texture.target , texture.channels);
            t->SetName(texture.name);
            texture.texture = t;
        }

        EXIT_FUNCTION_TRACE();
    }
    
    void ResourceHandler::UploadPrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos) {
        ENTER_FUNCTION_TRACE();
        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::LoadModels(ResourceMap<ModelResource>& models) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , model] : models) {
            Model* m = ynew Model(model.name , model.path);
            m->Load();
            model.model = m;
        }

        EXIT_FUNCTION_TRACE();
    }
    
    void ResourceHandler::CleanupShaders(ResourceMap<ShaderResource>& shaders) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , s] : engine_shaders) {
            if (s.shader != nullptr) ydelete s.shader;
        }
        shaders.clear();

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::CleanupTextures(ResourceMap<TextureResource>& textures) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , t] : textures) {
            if (t.texture != nullptr) ydelete t.texture; 
        }
        textures.clear();

        EXIT_FUNCTION_TRACE();
    }
    
    void ResourceHandler::CleanupFonts(ResourceMap<FontResource>& fonts) {
        ENTER_FUNCTION_TRACE();
        
        /// imgui deletes the fonts itself
        fonts.clear();

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::CleanupPrimitiveVAOs(ResourceMap<VertexArrayResource>& vaos) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , vao] : vaos) {
            if (vao.vao != nullptr) ydelete vao.vao;
        }
        vaos.clear();

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::CleanupModels(ResourceMap<ModelResource>& models) {
        ENTER_FUNCTION_TRACE();

        for (auto& [id , m] : models)
            if (m.model != nullptr) ydelete m.model;
        models.clear();

        EXIT_FUNCTION_TRACE();
    }

    ResourceHandler* ResourceHandler::Instance() {
        if (singleton == nullptr)
            singleton = ynew ResourceHandler;
        return singleton;
    }

    void ResourceHandler::Load() {
        ENTER_FUNCTION_TRACE();

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
        UploadPrimitiveVAOs(primitive_vaos);

        StoreModels(engine_model_dir , engine_models);
        StoreModels(app_model_dir , app_models);
        LoadModels(engine_models);
        LoadModels(app_models);

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::Offload() {
        ENTER_FUNCTION_TRACE();

        CleanupShaders(engine_shaders);
        CleanupShaders(app_shaders);
        CleanupTextures(engine_textures);
        CleanupTextures(app_textures);
        CleanupFonts(engine_fonts);
        CleanupPrimitiveVAOs(primitive_vaos);
        CleanupModels(engine_models);
        CleanupModels(app_models);
        
        msdfgen::deinitializeFreetype(freetype);

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::AddShader(
        const std::string& vert_path , const std::string& frag_path ,
        const std::string& geom_path , ResourceType type
    ) {
        ENTER_FUNCTION_TRACE_MSG(vert_path + " | " + frag_path + " | " + geom_path);
        
        // UUID id = Hash::FNV(vert_path + frag_path + geom_path);
        // if (!CheckID(id , vert_path , shaders)) return;
        // shaders[id].shader = ynew Shader(vert_path , frag_path , geom_path);
        // if (!shaders[id].shader->Compile()) {
        //     ENGINE_WARN("Failed to compile shader :: [{0}] | [{1}] | [{2}]" , vert_path , frag_path , geom_path);
        //     ydelete shader;
        //     return;
        // }

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::AddTexture(const std::string& path , ResourceType type) {
        ENTER_FUNCTION_TRACE_MSG(path);

        // UUID id = Hash::FNV(path);
        // if (!CheckID(id , path , textures)) return;
        // textures[id].texture = ynew Texture(path);
        // textures[id].texture->Load();

        EXIT_FUNCTION_TRACE();
    }
    
    void ResourceHandler::AddTexture(const std::string& name , Texture* texture , ResourceType type) {
        ENTER_FUNCTION_TRACE_MSG(name);

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

        EXIT_FUNCTION_TRACE();
    } 

    void ResourceHandler::AddModel(const std::string& path , ResourceType type) {
        ENTER_FUNCTION_TRACE_MSG(path);

        // UUID id = Hash::FNV(path);
        // if (!CheckID(id , path , models)) return;
        // models[id].model = ynew Model(path);
        // models[id].model->Load();

        EXIT_FUNCTION_TRACE();
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

    VertexArray* ResourceHandler::GetPrimitiveVAO(const std::string& name) {
        for (auto& [id , vao] : primitive_vaos)
            if (vao.name == name) return vao.vao;
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
        ENTER_FUNCTION_TRACE();

        CleanupShaders(engine_shaders);
        CleanupShaders(app_shaders);
        StoreShaders(engine_shader_dir , engine_shaders);
        StoreShaders(app_shader_dir , app_shaders);
        CompileShaders(engine_shaders);
        CompileShaders(app_shaders);
        shaders_reloaded = true;

        EXIT_FUNCTION_TRACE();
    }

    void ResourceHandler::Cleanup() {
        ENTER_FUNCTION_TRACE();

        if (singleton != nullptr) ydelete singleton;

        EXIT_FUNCTION_TRACE();
    }

}