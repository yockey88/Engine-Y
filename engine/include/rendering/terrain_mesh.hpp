#ifndef ENGINEY_TERRAIN_MESH_HPP
#define ENGINEY_TERRAIN_MESH_HPP

#include <glm/glm.hpp>

#include "rendering/vertex_array.hpp"

namespace EngineY {

    class TerrainMesh {

        glm::ivec2 dimensions;

        VertexArray* vao = nullptr;

        public:
            TerrainMesh(glm::ivec2 dimensions)
                : dimensions(dimensions) {}
            ~TerrainMesh() {}

            void GenerateMesh();
    };

}

#endif // !YE_TERRAIN_MESH_HPP