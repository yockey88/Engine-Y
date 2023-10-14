#ifndef YE_TERRAIN_GENERATOR_HPP
#define YE_TERRAIN_GENERATOR_HPP

#include <glm/glm.hpp>

#include "noise2D.hpp"
#include "height_map2D.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/camera.hpp"

namespace YE {

    class TerrainGenerator {
        VertexArray* vao = nullptr;
        Shader* shader = nullptr;
        HeightMap2D* height_map = nullptr;
        NoiseGenerator noise_generator = nullptr;
        ColorEditor color_editor = nullptr;

        glm::ivec2 dimensions;

        float terrain_scale = 1.f;
        bool generated = false;

        void GenerateHeightMap(const GeneratorArgs& args);
        void GenerateMesh();

        void Destroy();
        void DestroyHeightMap();
        void DestroyMesh();

        public:
            TerrainGenerator(const glm::ivec2& dimensions , NoiseGenerator generator = Noise2D::PerlinNoise)
                            : dimensions(dimensions) , noise_generator(generator) {}
            ~TerrainGenerator();

            void Generate(const GeneratorArgs& args);

            void Draw(Camera* camera = nullptr , DrawMode mode = DrawMode::TRIANGLES);

            inline HeightMap2D* HeightMap() const { return height_map; }
            inline void AttachShader(Shader* shader) { this->shader = shader; }
            inline void AttachNoiseGenerator(NoiseGenerator generator) { noise_generator = generator; }
            inline void AttachHeightTextureColorEditor(ColorEditor editor) { color_editor = editor; }
            inline void BindHeightMap(uint32_t slot = 0) { height_map->Bind(slot); }
            inline void UnbindHeightMap(uint32_t slot = 0) { height_map->Unbind(slot); }
            inline void SetScale(float scale) { terrain_scale = scale; }

    };

}

#endif // !YE_TERRAIN_GENERATOR_HPP