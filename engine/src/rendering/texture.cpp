#include "rendering/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/log.hpp"

namespace EngineY {
    
    Texture::Texture(
        const glm::ivec2& size , void* pixels , 
        ChannelType channels , TargetType target
    ) {
        this->size = size;
        this->pixels = reinterpret_cast<uint8_t*>(pixels);
        this->channels = channels;
        this->target = target;
        
        glGenTextures(1 , &texture);
        glBindTexture(target , texture);

        glTexParameteri(target , GL_TEXTURE_WRAP_S , GL_REPEAT);
        glTexParameteri(target , GL_TEXTURE_WRAP_T , GL_REPEAT);

        glTexImage2D(target , 0 , channels , size.x , size.y , 0 , channels , GL_UNSIGNED_BYTE , pixels);

        glTexParameteri(target , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
        glTexParameteri(target , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        
        glGenerateMipmap(target);

        loaded = true;
    }

    Texture::~Texture() {
        stbi_image_free(pixels);
        pixels = nullptr;

        glDeleteTextures(1 , &texture);
    }

    void Texture::Load(TargetType target , ChannelType channels) {
        if (loaded)
            return;

        this->target = target;
        this->channels = channels;
        
        glGenTextures(1 , &texture);
        glBindTexture(target , texture);

        int num_channels = 0;
        pixels = stbi_load(path.c_str() , &size.x , &size.y , &num_channels , 0);

        if (pixels == nullptr) {
            ENGINE_ERROR("Failed to load texture :: {0}" , path);
            ENGINE_WARN("Using default texture");

            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);
            
            float def_pixels[] = {
                1.f , 1.f , 1.f ,   1.f , 0.f , 0.f ,   0.f , 1.f , 0.f ,   0.f , 0.f , 1.f ,
                0.f , 0.f , 1.f ,   1.f , 1.f , 1.f ,   1.f , 0.f , 0.f ,   0.f , 1.f , 0.f ,
                0.f , 1.f , 0.f ,   0.f , 0.f , 1.f ,   1.f , 1.f , 1.f ,   1.f , 0.f , 0.f ,
                1.f , 0.f , 0.f ,   0.f , 1.f , 0.f ,   0.f , 0.f , 1.f ,   1.f , 1.f , 1.f ,
            };
            channels = RGB;
            size = { 4 , 4 };
            int def_num_channels = 3;

            glTexImage2D(GL_TEXTURE_2D , 0 , channels , size.x , size.y , 0 , channels , GL_FLOAT , def_pixels);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST);
        } else {
            glTexParameteri(target , GL_TEXTURE_WRAP_S , GL_REPEAT);
            glTexParameteri(target , GL_TEXTURE_WRAP_T , GL_REPEAT);
           
            if (num_channels == 1) {
                channels = RED;
            } else if (num_channels == 3) {
                channels = RGB;
            } else if (num_channels == 4) {
                channels = RGBA;
            } else {
                ENGINE_ASSERT(false , "Invalid number of channels");
            }

            glTexImage2D(target , 0 , channels , size.x , size.y , 0 , channels , GL_UNSIGNED_BYTE , pixels);

            glTexParameteri(target , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
            glTexParameteri(target , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            
            glGenerateMipmap(target);
        }

        loaded = true;
    }
            
    void Texture::SetFilterType(FilterType filter) {
        this->filter = filter;
        glBindTexture(target , texture);
        glTexParameteri(target , GL_TEXTURE_MIN_FILTER , filter);
        glTexParameteri(target , GL_TEXTURE_MAG_FILTER , filter);
    }

    void Texture::SetTextureType(TextureType type) {
        this->type = type;
        // switch (type) {
        //     case TextureType::diffuse:
        //         SetFilterType(linear);
        //         break;
        //     case TextureType::specular:
        //         SetFilterType(linear);
        //         break;
        //     case TextureType::normal:
        //         SetFilterType(nearest);
        //         break;
        //     case TextureType::height:
        //         SetFilterType(nearest);
        //         break;
        //     default:
        //         ENGINE_ASSERT(false , "Invalid texture type");
        //         break;
        // }
    }

    void Texture::Bind(uint32_t pos) const {
        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(target , texture);
    }

    void Texture::Unbind(uint32_t pos) const {
        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(target , 0);
    }

    std::string Texture::GetTypeName() const {
        switch (type) {
            case TextureType::diffuse: return "diffuse";
            case TextureType::specular: return "specular";
            case TextureType::normal: return "normal";
            case TextureType::height: return "height";
            default:
                ENGINE_ASSERT(false , "Invalid texture type");
                return "";
        }
    }
}