#include "rendering/primitive_vao_data.hpp"

#include <cmath>
#include <vector>

namespace EngineY {

namespace primitives {

const std::vector<float> VertexBuilder::GenerateQuadVerts(uint32_t D1 , uint32_t D2) {
    return {
        0.5f ,  0.5f , 0.f , 
        0.5f , -0.5f , 0.f ,
        0.5f , -0.5f , 0.f ,
        0.5f ,  0.5f , 0.f
    };
}

const std::vector<uint32_t> VertexBuilder::GetQuadIndices(uint32_t D1 , uint32_t D2) {
    return {
        0 , 1 , 3 ,
        1 , 2 , 3
    };
}

const std::vector<float> VertexBuilder::GenerateCubeVerts(uint32_t D1 , uint32_t D2) {
    return {};
}

const std::vector<uint32_t> VertexBuilder::GetCubeIndices(uint32_t D1 , uint32_t D2) {
    return {};
}

const std::vector<float> VertexBuilder::BuildIcosahedronVerts() {
    std::vector<float> res(12 * 3);

    const float pi = std::atanf(1.f) * 4.f;
    
    const float hangle = pi / 180.0f * 72.0f; 
    const float vangle = std::atanf(0.5f);

    float hangleu = -pi / 2 - hangle / 2;
    float hangled = -pi / 2;

    res[0] = 0.f;
    res[1] = 0.f;
    res[2] = 0.5f;

    uint32_t i1;
    uint32_t i2;
    
    for (uint32_t i = 1; i <= 5; ++i) {
        i1 = i * 3;
        i2 = (i + 5) * 3;
    
        res[i1]     = std::cosf(vangle) * std::cosf(hangleu);
        res[i1 + 1] = std::cosf(vangle) * std::sinf(hangleu);
        res[i1 + 2] = std::sinf(vangle);

        res[i2]     = std::cosf(vangle) * std::cosf(hangled);
        res[i2 + 1] = std::cosf(vangle) * std::sinf(hangled);
        res[i2 + 2] = -0.5f * std::sinf(vangle);
    
        hangleu += hangle;
        hangled += hangle;
    }
    
    // the last bottom vertex at (0, 0, -r)
    i1 = 11 * 3;
    res[i1] = 0;
    res[i1 + 1] = 0;
    res[i1 + 2] = -0.5f;

    return res;
}

const std::vector<float> VertexBuilder::GenerateIcosahedronVerts(uint32_t D1 , uint32_t D2) {
    std::vector<float> res;

    auto AddVerts = 
        [&res](float* v1 , float* v2 , float* v3) {
            res.push_back(v1[0]); res.push_back(v1[1]); res.push_back(v1[2]);
            res.push_back(v2[0]); res.push_back(v2[1]); res.push_back(v2[2]);
            res.push_back(v3[0]); res.push_back(v3[1]); res.push_back(v3[2]);
        };

    std::vector<float> tmp = BuildIcosahedronVerts();

    float* v0 = &tmp[0];
    float* v11 = &tmp[11 * 3];
    for (uint32_t i = 1; i <= 5; ++i) {
        float* v1 = &tmp[i * 3];
        float* v2 = i < 5 ?
            &tmp[(i + 1) * 3] : 
            &tmp[3];

        float* v3 = &tmp[(i + 5) * 3];
        float* v4 = (i + 5) < 10 ?
            &tmp[(i + 6) * 3] : 
            &tmp[6 * 3];

        AddVerts(v0 , v1  , v2);
        AddVerts(v1 , v3  , v2);
        AddVerts(v2 , v3  , v4);
        AddVerts(v3 , v11 , v4);
    }

    return res;
}

const std::vector<uint32_t> VertexBuilder::GetIcosahedronIndices(uint32_t D1 , uint32_t D2) {
    std::vector<uint32_t> indices;
    
    auto AddIndices = 
        [&indices](uint32_t i1 , uint32_t i2 , uint32_t i3) {
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        };

    uint32_t i0 = 0; 
    for (uint32_t i = 1; i <= 5; ++i) {
        AddIndices(i0 , i0 + 1 , i0 + 2);
        AddIndices(i0 + 3 , i0 + 4 , i0 + 5);
        AddIndices(i0 + 6 , i0 + 7 , i0 + 8);
        AddIndices(i0 + 9 , i0 + 10 , i0 + 11);
        i0 += 12;
    }

    return indices;
}

const std::vector<float> VertexBuilder::GetPrimitiveVertices(UUID id , uint32_t arg1 , uint32_t arg2) {
    uint32_t i = 0;
    for (auto& id : primitive_ids) {
        if (id == id) {
            return primitive_vert_builders[i](arg1 , arg2);
        }
        ++i;
    }

    return {};
}

const std::vector<uint32_t> VertexBuilder::GetPrimitiveIndices(UUID id , uint32_t arg1 , uint32_t arg2) {
    uint32_t i = 0;
    for (auto& id : primitive_ids) {
        if (id == id) {
            return primitive_index_builders[i](arg1 , arg2);
        }
        ++i;
    }

    return {};
}

} // namespace primitives

} // namespace EngineY
