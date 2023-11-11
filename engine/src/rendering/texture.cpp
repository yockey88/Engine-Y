#include "rendering/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"

namespace YE {

    Texture::~Texture() {
        stbi_image_free(pixels);
        pixels = nullptr;

        glDeleteTextures(1 , &texture);
    }

    void Texture::Load(TargetType target , ChannelType channels) {
        this->target = target;
        this->channels = channels;
        
        glGenTextures(1 , &texture);

        glBindTexture(GL_TEXTURE_2D , texture);
        
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);

        int num_channels = 0;
        pixels = stbi_load(path.c_str() , &size.x , &size.y , &num_channels , 0);

        if (pixels == nullptr) {
            ENGINE_ERROR("Failed to load texture :: {0}" , path);
            ENGINE_WARN("Using default texture");

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

            if (num_channels == 1) {
                channels = RED;
            } else if (num_channels == 3) {
                channels = RGB;
            } else if (num_channels == 4) {
                channels = RGBA;
            } else {
                YE_CRITICAL_ASSERTION(false , "Invalid number of channels");
            }

            glTexImage2D(GL_TEXTURE_2D , 0 , channels , size.x , size.y , 0 , channels , GL_UNSIGNED_BYTE , pixels);

            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            
            glGenerateMipmap(GL_TEXTURE_2D);
        }
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
        //         YE_CRITICAL_ASSERTION(false , "Invalid texture type");
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
                YE_CRITICAL_ASSERTION(false , "Invalid texture type");
                return "";
        }
    }
}