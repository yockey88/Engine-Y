#include "rendering/terrain_mesh.hpp"

#include "log.hpp"

namespace YE {

    void TerrainMesh::GenerateMesh() {
        YE_CRITICAL_ASSERTION(dimensions.x > 0 && dimensions.y > 0 , "Invalid dimensions");
        YE_CRITICAL_ASSERTION(dimensions.x % 2 == 0 && dimensions.y % 2 == 0 , "Dimensions must be even");

        for (uint32_t i = 0; i < dimensions.x; i++) {
            for (uint32_t j = 0; j < dimensions.y; j++) {

            }
        }
    }

}