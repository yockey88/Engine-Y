#ifndef ENGINEY_HEIGHT_MAP_HPP
#define ENGINEY_HEIGHT_MAP_HPP

#include <glm/glm.hpp>

namespace EngineY {

    // struct GeneratorArgs {
    //     float amplitude = 1.f;
    //     float frequency = 1.f;
    //     float persistence = 1.f;
    //     uint32_t octaves = 1;
    // };

    // typedef float(*NoiseGenerator)(const glm::vec2& position , GeneratorArgs args);
    // typedef uint8_t*(*GeneratorCallback)(const glm::ivec2& dimensions , GeneratorArgs args);

    // class HeightMap {
    //     NoiseGenerator noise_generator = PerlinNoise;
        
    //     glm::ivec2  dimensions;
    //     float* height_data = nullptr;
    //     uint8_t* pixels = nullptr;

    //     public:
    //         HeightMap(const glm::ivec2& dimensions)
    //             : dimensions(dimensions) {}
    //         ~HeightMap() {}

    //         void GenerateHeightMap(GeneratorCallback callback , GeneratorArgs args) {}

    //         inline void AttachNoiseGenerator(NoiseGenerator generator) { noise_generator = generator; }
    // };

}

#endif // !YE_HEIGHT_MAP_HPP