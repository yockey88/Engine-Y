#include "world/terrain_generator.hpp"

#include <vector>

#include <glad/glad.h>

#include "log.hpp"

namespace YE {

    void TerrainGenerator::GenerateHeightMap(const GeneratorArgs& args) {
        height_map = ynew HeightMap2D(dimensions);
        height_map->AttachNoiseGenerator(noise_generator);
        height_map->AttachColorEditor(color_editor);
        height_map->GenerateHeightMap(args);
    }

    void TerrainGenerator::GenerateMesh() {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for (int32_t z = 0; z < dimensions.y; z++) {
            for (int32_t x = 0; x < dimensions.x; x++) {
                vertices.push_back(x);
                vertices.push_back(height_map->HeightAt({ x , z }));
                vertices.push_back(z);
                
                vertices.push_back((float)x / (float)dimensions.x);
                vertices.push_back((float)z / (float)dimensions.y);
            }
        }

        for (uint32_t z = 0; z < dimensions.y - 1; ++z) {
            for (uint32_t x = 0; x < dimensions.x - 1; ++x) {
                uint32_t index_bl = (z * dimensions.x) + x;
                uint32_t index_tl = ((z + 1) * dimensions.x) + x;
                uint32_t index_tr = ((z + 1) * dimensions.x) + (x + 1);
                uint32_t index_br = (z * dimensions.x) + (x + 1);

                // top left triangle
                indices.push_back(index_bl);
                indices.push_back(index_tl);
                indices.push_back(index_tr);

                // bottom right triangle
                indices.push_back(index_tr);
                indices.push_back(index_br);
                indices.push_back(index_bl);
            }
        }

        vao = ynew VertexArray(vertices , indices , { 3 , 2 });
        vao->Upload();

        generated = true;
    }
    
    void TerrainGenerator::Destroy() {
        if (!generated) 
            return;

        DestroyHeightMap();
        DestroyMesh();

        generated = false;
    }

    void TerrainGenerator::DestroyHeightMap() {
        if (height_map != nullptr)
            ydelete height_map;
        height_map = nullptr;
    }

    void TerrainGenerator::DestroyMesh() {
        if (vao != nullptr)
            ydelete vao;
        vao = nullptr;
    }

    TerrainGenerator::~TerrainGenerator() {
        Destroy();
    }

    void TerrainGenerator::Generate(const GeneratorArgs& args) {
        if (generated)
            Destroy();

        GenerateHeightMap(args);
        GenerateMesh();
    }
    
    void TerrainGenerator::Draw(Camera* camera , DrawMode mode) {
        if (shader == nullptr) {
            YE_ERROR("TerrainGenerator::Draw() : shader is nullptr!");
            return;
        }

        shader->Bind();
        if (camera != nullptr) {
            shader->SetUniformInt("camera_active" , 1);
            shader->SetUniformMat4("view" , camera->View());
            shader->SetUniformMat4("proj" , camera->Projection());
            shader->SetUniformVec3("view_pos" , camera->Position());
        } else {
            shader->SetUniformInt("camera_active" , 0);
        }
        shader->SetUniformFloat("world_scale" , terrain_scale);
        vao->Draw(mode);
        shader->Unbind();
    }

}