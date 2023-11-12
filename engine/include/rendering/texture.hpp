#ifndef YE_TEXTURE_HPP
#define YE_TEXTURE_HPP

#include <string>
#include <utility>

#include <glm/glm.hpp>

#include <glad/glad.h>

#define PIXEL_INDEX(i , j , width) (i*width + j)

namespace YE {

    class Texture;
    
    enum FilterType {
        nearest = GL_NEAREST ,
        linear = GL_LINEAR
    };

    enum TextureType {
        diffuse , 
        specular ,
        normal ,
        height
    };
    
    enum ChannelType {
        RED = GL_RED ,
        RGB = GL_RGB ,
        RGBA = GL_RGBA
    };
    
    enum TargetType {
        TEX_1D = GL_TEXTURE_1D ,
        TEX_2D = GL_TEXTURE_2D ,
        TEX_3D = GL_TEXTURE_3D ,
        TEX_1D_ARR = GL_TEXTURE_1D_ARRAY ,
        TEX_2D_ARR = GL_TEXTURE_2D_ARRAY ,
        TEX_RECT = GL_TEXTURE_RECTANGLE ,
        TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP ,
        TEX_CUBE_MAP_ARR = GL_TEXTURE_CUBE_MAP_ARRAY ,
        TEX_BUFFER = GL_TEXTURE_BUFFER ,
        TEX_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE ,
        TEX_2D_MULTISAMPLE_ARR = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };

    struct TextureResource {
        std::string name;
        std::string filename;
        std::string path;

        ChannelType channels = ChannelType::RGB;
        TargetType target = TargetType::TEX_2D;

        Texture* texture = nullptr;

        TextureResource() {}
    };

    class Texture {

        ChannelType channels = ChannelType::RGB;
        TargetType target = TargetType::TEX_2D;
        FilterType filter = linear;
        TextureType type = TextureType::diffuse;

        uint32_t texture = 0;
        glm::ivec2 size;

        uint8_t* pixels = nullptr;

        bool loaded = false;

        std::string name;
        std::string path;
        
        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;
        
        public:
            Texture(
                const glm::ivec2& size , void* pixels , 
                ChannelType channels = RGB , TargetType target = TEX_2D
            ); 
            Texture(const std::string& path) 
                : path(path) {}
            ~Texture();

            void Load(TargetType target = TEX_2D , ChannelType channels = RGB);

            void SetFilterType(FilterType filter);
            void SetTextureType(TextureType type);

            void Bind(uint32_t pos = 0) const;
            void Unbind(uint32_t pos = 0) const;

            std::string GetTypeName() const;

            inline const glm::ivec2& Size() const { return size; }
            inline std::string Name() const { return name; }
            inline void SetName(const std::string& name) { this->name = name; }
    };

    class CubeMap {
        std::string path;

        public:
            CubeMap(const std::string& path) 
                : path(path) {}
    };

}

#endif // !YE_TEXTURE_HPP