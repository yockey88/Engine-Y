#include "world/height_map2D.hpp"

#include "glad/glad.h"

#include "log.hpp"
#include "core/RNG.hpp"

namespace YE {
    
    void HeightMap2D::CreateTexture() {
        glGenTextures(1 , &texture);

        glBindTexture(GL_TEXTURE_2D , texture);
        
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);
    }

    void HeightMap2D::Generate(uint32_t num_channels , const GeneratorArgs& args) {
        uint32_t height_dim = dimensions.x * dimensions.y; // regular dimensions
        uint32_t channel_width = dimensions.x * num_channels; 
        uint32_t text_dim = channel_width * dimensions.y; // dimensions accounting for number of channels

        RNG::RngEngineF32 rng(0.f , 1.f);

        height_data = ynew float[height_dim]; // regular height data
        height_texture_data = ynew float[text_dim]; // texture height data with r/rgb/rgba channels
        for (uint32_t i = 0; i < text_dim; ++i)
            height_texture_data[i] = 0.f;

        for (int32_t y = 0; y < dimensions.y; ++y) {
            for (int32_t x = 0; x < dimensions.x; ++x) {
                uint32_t h_index = (y * dimensions.x) + x;
                uint32_t ht_index = (y * channel_width) + (x * num_channels);

                glm::vec2 pos(x + rng() , y + rng()); 

                float n = noise_generator(pos , args);
                height_data[h_index] = n;

                if (n < min_height)
                    min_height = n;
                
                if (n > max_height)
                    max_height = n;

                n += 1.f;
                n /= 2.f;

                glm::vec3 rgb = glm::vec3(n);

                if (color_editor != nullptr)
                    color_editor(rgb , n);

                height_texture_data[ht_index] = rgb.r;
                height_texture_data[ht_index + 1] = rgb.g;
                height_texture_data[ht_index + 2] = rgb.b;
                height_texture_data[ht_index + 3] = 1.f;
                
            }
        }
    }
    
    void HeightMap2D::GenerateTexture() {
        glTexImage2D(GL_TEXTURE_2D , 0 , channel_type , dimensions.x , dimensions.y , 0 , channel_type , GL_FLOAT , height_texture_data);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST);

        generated = true;
    }

    void HeightMap2D::DestroyHeightMap() {
        if (height_data != nullptr) {
            ydelete[] height_data;
            height_data = nullptr;
        }
        if (height_texture_data != nullptr) {
            ydelete[] height_texture_data;
            height_texture_data = nullptr;
        }
        glDeleteTextures(1 , &texture);

        generated = false;
    }

    HeightMap2D::~HeightMap2D() {
        DestroyHeightMap();
    }

    void HeightMap2D::GenerateHeightMap(GeneratorArgs args) {
        if (noise_generator == nullptr) {
            ENGINE_ERROR("HeightMap2D::GenerateHeightMap() :: noise generator is nullptr");
            return;
        }

        if (generated)
            DestroyHeightMap();

        CreateTexture();

        num_channels = 0;
        switch (channel_type) {   
            case YE::ChannelType::RED: num_channels = 1; break;    
            case YE::ChannelType::RGB: num_channels = 3; break;
            case YE::ChannelType::RGBA: num_channels = 4; break;
            default: YE_CRITICAL_ASSERTION(false , "Invalid number of channels for height map");
        }

        Generate(num_channels , args);

        GenerateTexture();
    }

    void HeightMap2D::Bind(uint32_t slot) {
        if (generated) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D , texture);
        }
    }

    void HeightMap2D::Unbind(uint32_t slot) {
        if (generated) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D , 0);
        }
    }
    
    float HeightMap2D::HeightAt(const glm::ivec2& position) const {
        if (generated) {
            uint32_t index = (position.y * dimensions.x) + position.x;
            return height_data[index];
        }
        return 0.f;
    }

}