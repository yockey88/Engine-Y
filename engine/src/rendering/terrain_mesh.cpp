#include "rendering/terrain_mesh.hpp"

#include "core/log.hpp"

namespace EngineY {

    void TerrainMesh::GenerateMesh() {
        ENGINE_ASSERT(dimensions.x > 0 && dimensions.y > 0 , "Invalid dimensions");
        ENGINE_ASSERT(dimensions.x % 2 == 0 && dimensions.y % 2 == 0 , "Dimensions must be even");

        for (uint32_t i = 0; i < dimensions.x; i++) {
            for (uint32_t j = 0; j < dimensions.y; j++) {

            }
        }
    }

}