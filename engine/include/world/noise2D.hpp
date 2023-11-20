#ifndef ENGINEY_NOISE2D_HPP
#define ENGINEY_NOISE2D_HPP

#include <glm/glm.hpp>

namespace EngineY {

    struct GeneratorArgs {
        float amplitude = 1.f;
        float frequency = 1.f;
        float persistence = 1.f;
        uint32_t octaves = 1;
    };

}

// for switching out noise generators
typedef float(*NoiseGenerator)(const glm::vec2& position ,  EngineY::GeneratorArgs arguments);

namespace EngineY {

    class Noise2D {
        public:
            static float PerlinNoise(const glm::vec2& pos , GeneratorArgs args);
            static float FractalBrownianMotion(const glm::vec2& pos , GeneratorArgs args);
    };

}

#endif // !YE_NOISE2D_HPP