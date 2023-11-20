#ifndef ENGINEY_HEIGHT_MAP_HPP
#define ENGINEY_HEIGHT_MAP_HPP

#include <glm/glm.hpp>

#include "noise2D.hpp"
#include "rendering/texture.hpp"

namespace EngineY {

    struct GeneratorArgs;

}

// for editing final color of height map if choosing colors based on height
typedef void(*ColorEditor)(glm::vec3& rgb , float nosie_val);

namespace EngineY {

    class HeightMap2D {
        NoiseGenerator noise_generator = nullptr;
        ColorEditor color_editor = nullptr;
         EngineY::ChannelType channel_type;
            
        glm::ivec2  dimensions;

        uint32_t min_height = std::numeric_limits<uint32_t>::max();
        uint32_t max_height = std::numeric_limits<uint32_t>::min();

        uint32_t texture = 0;
        uint32_t num_channels = 0;
        float* height_data = nullptr;
        float* height_texture_data = nullptr;

        bool generated = false;

        void CreateTexture();
        void Generate(uint32_t num_channels , const GeneratorArgs& args);
        void GenerateTexture();
        void DestroyHeightMap();

        public:
            HeightMap2D(const glm::ivec2& dimensions,  EngineY::ChannelType type =  EngineY::ChannelType::RGBA)
                : dimensions(dimensions) , channel_type(type) {}
            ~HeightMap2D();

            float HeightAt(const glm::ivec2& position) const;
            void GenerateHeightMap(GeneratorArgs args);
            void Bind(uint32_t slot = 0);
            void Unbind(uint32_t slot = 0);

            inline const glm::ivec2& Dimensions() const { return dimensions; }
            inline const glm::ivec2 MinMaxHeight() const { return { min_height , max_height }; }
            inline const uint32_t MinHeight() const { return min_height; }
            inline const uint32_t MaxHeight() const { return max_height; }
            inline uint32_t Texture() const { return texture; }
            inline void AttachNoiseGenerator(NoiseGenerator generator) { noise_generator = generator; }
            inline void AttachColorEditor(ColorEditor editor) { color_editor = editor; }
    };

}

#endif // !YE_HEIGHT_MAP_HPP