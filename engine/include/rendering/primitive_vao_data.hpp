#ifndef ENGINEY_PRIMITIVE_VAO_DATA_HPP
#define ENGINEY_PRIMITIVE_VAO_DATA_HPP

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "core/UUID.hpp"
#include "core/hash.hpp"
#include "rendering/vertex.hpp"

namespace EngineY {

namespace primitives {

    static const class VertData {
        // quad verts
        glm::vec3  qp1{  1.f ,  1.f , 0.0f };
        glm::vec3  qp2{  1.f , -1.f , 0.0f };
        glm::vec3  qp3{ -1.f , -1.f , 0.0f };
        glm::vec3  qp4{ -1.f ,  1.f , 0.0f };

        // top
        glm::vec3  p1{ -0.5f,  0.5f,  0.5f };
        glm::vec3  p2{ -0.5f,  0.5f, -0.5f };
        glm::vec3  p3{  0.5f,  0.5f, -0.5f };
        glm::vec3  p4{ -0.5f,  0.5f,  0.5f };
        glm::vec3  p5{  0.5f,  0.5f,  0.5f };
        glm::vec3  p6{  0.5f,  0.5f, -0.5f };

        // bottom
        glm::vec3  p7{ -0.5f, -0.5f,  0.5f };
        glm::vec3  p8{ -0.5f, -0.5f, -0.5f };
        glm::vec3  p9{  0.5f, -0.5f, -0.5f };
        glm::vec3 p10{ -0.5f, -0.5f,  0.5f };
        glm::vec3 p11{  0.5f, -0.5f,  0.5f };
        glm::vec3 p12{  0.5f, -0.5f, -0.5f };

        // left
        glm::vec3 p13{ -0.5f, -0.5f,  0.5f };
        glm::vec3 p14{ -0.5f,  0.5f,  0.5f };
        glm::vec3 p15{ -0.5f,  0.5f, -0.5f };
        glm::vec3 p16{ -0.5f, -0.5f,  0.5f };
        glm::vec3 p17{ -0.5f, -0.5f, -0.5f };
        glm::vec3 p18{ -0.5f,  0.5f, -0.5f };

        // right
        glm::vec3 p19{  0.5f, -0.5f,  0.5f };
        glm::vec3 p20{  0.5f,  0.5f,  0.5f };
        glm::vec3 p21{  0.5f,  0.5f, -0.5f };
        glm::vec3 p22{  0.5f, -0.5f,  0.5f };
        glm::vec3 p23{  0.5f, -0.5f, -0.5f };
        glm::vec3 p24{  0.5f,  0.5f, -0.5f };

        // front
        glm::vec3 p25{ -0.5f, -0.5f,  0.5f };
        glm::vec3 p26{ -0.5f,  0.5f,  0.5f };
        glm::vec3 p27{  0.5f,  0.5f,  0.5f };
        glm::vec3 p28{ -0.5f, -0.5f,  0.5f };
        glm::vec3 p29{  0.5f, -0.5f,  0.5f };
        glm::vec3 p30{  0.5f,  0.5f,  0.5f };

        // back
        glm::vec3 p31{ -0.5f, -0.5f, -0.5f };
        glm::vec3 p32{ -0.5f,  0.5f, -0.5f };
        glm::vec3 p33{  0.5f,  0.5f, -0.5f };
        glm::vec3 p34{ -0.5f, -0.5f, -0.5f };
        glm::vec3 p35{  0.5f, -0.5f, -0.5f };
        glm::vec3 p36{  0.5f,  0.5f, -0.5f };

        glm::vec3  c1{ 1.f , 0.f , 0.f };
        glm::vec3  c2{ 0.f , 1.f , 0.f };
        glm::vec3  c3{ 0.f , 0.f , 1.f };
        glm::vec3  c4{ 1.f , 1.f , 1.f };

        glm::vec3 rnorm1{  1.f ,  1.f , 0.f };
        glm::vec3 rnorm2{  1.f , -1.f , 0.f };
        glm::vec3 rnorm3{ -1.f , -1.f , 0.f };
        glm::vec3 rnorm4{ -1.f ,  1.f , 0.f };

        glm::vec3  norm1{ -1.f ,  1.f ,  1.f };
        glm::vec3  norm2{ -1.f ,  1.f , -1.f };
        glm::vec3  norm3{  1.f ,  1.f , -1.f };
        glm::vec3  norm4{  1.f ,  1.f ,  1.f };
        glm::vec3  norm5{ -1.f , -1.f ,  1.f };
        glm::vec3  norm6{ -1.f , -1.f , -1.f };
        glm::vec3  norm7{  1.f , -1.f , -1.f };
        glm::vec3  norm8{  1.f , -1.f ,  1.f };

        glm::vec3 tan1{ 1.f , 0.f , 0.f };
        glm::vec3 tan2{ 1.f , 0.f , 0.f };
        glm::vec3 tan3{ 1.f , 0.f , 0.f };
        glm::vec3 tan4{ 1.f , 0.f , 0.f };

        glm::vec3 bitan1{ 0.f , 1.f , 0.f };
        glm::vec3 bitan2{ 0.f , 1.f , 0.f };
        glm::vec3 bitan3{ 0.f , 1.f , 0.f };
        glm::vec3 bitan4{ 0.f , 1.f , 0.f };

        glm::vec2  t1{ 0.f , 0.f };
        glm::vec2  t2{ 0.f , 1.f };
        glm::vec2  t3{ 1.f , 1.f };
        glm::vec2  t4{ 1.f , 0.f };

        float opacity = 1.0f;

        public:
            // quad verts 
             EngineY::Vertex qv1 =  EngineY::Vertex(qp1 , c1  , rnorm1 , tan1 , bitan1 , t1  , opacity);
             EngineY::Vertex qv2 =  EngineY::Vertex(qp2 , c2  , rnorm2 , tan2 , bitan2 , t2  , opacity);
             EngineY::Vertex qv3 =  EngineY::Vertex(qp3 , c3  , rnorm3 , tan3 , bitan3 , t3  , opacity);
             EngineY::Vertex qv4 =  EngineY::Vertex(qp4 , c4  , rnorm4 , tan4 , bitan4 , t4  , opacity);

            // top
             EngineY::Vertex  v1 =  EngineY::Vertex(p1 , c1  , norm1 , tan1 , bitan1 , t1  , opacity);
             EngineY::Vertex  v2 =  EngineY::Vertex(p2 , c2  , norm2 , tan2 , bitan2 , t2  , opacity);
             EngineY::Vertex  v3 =  EngineY::Vertex(p3 , c3  , norm3 , tan3 , bitan3 , t3  , opacity);
             EngineY::Vertex  v4 =  EngineY::Vertex(p4 , c4  , norm1 , tan4 , bitan4 , t1  , opacity);
             EngineY::Vertex  v5 =  EngineY::Vertex(p5 , c1  , norm4 , tan1 , bitan1 , t4  , opacity);
             EngineY::Vertex  v6 =  EngineY::Vertex(p6 , c2  , norm3 , tan2 , bitan2 , t3  , opacity);

            // bottom
             EngineY::Vertex  v7 =  EngineY::Vertex(p7  , c3  , norm5 , tan3 , bitan3 , t1  , opacity);
             EngineY::Vertex  v8 =  EngineY::Vertex(p8  , c4  , norm6 , tan4 , bitan4 , t2  , opacity);
             EngineY::Vertex  v9 =  EngineY::Vertex(p9  , c1  , norm7 , tan1 , bitan1 , t3  , opacity);
             EngineY::Vertex v10 =  EngineY::Vertex(p10 , c2  , norm5 , tan2 , bitan2 , t1  , opacity);
             EngineY::Vertex v11 =  EngineY::Vertex(p11 , c2  , norm8 , tan3 , bitan3 , t4  , opacity);
             EngineY::Vertex v12 =  EngineY::Vertex(p12 , c1  , norm7 , tan4 , bitan4 , t3  , opacity);

            // left 
             EngineY::Vertex v13 =  EngineY::Vertex(p13 , c4  , norm5 , tan1 , bitan1 , t4  , opacity);
             EngineY::Vertex v14 =  EngineY::Vertex(p14 , c3  , norm1 , tan2 , bitan2 , t3  , opacity);
             EngineY::Vertex v15 =  EngineY::Vertex(p15 , c3  , norm2 , tan3 , bitan3 , t2  , opacity);
             EngineY::Vertex v16 =  EngineY::Vertex(p16 , c4  , norm5 , tan4 , bitan4 , t4  , opacity);
             EngineY::Vertex v17 =  EngineY::Vertex(p17 , c1  , norm6 , tan1 , bitan1 , t1  , opacity);
             EngineY::Vertex v18 =  EngineY::Vertex(p18 , c1  , norm2 , tan2 , bitan2 , t2  , opacity);

            // right
             EngineY::Vertex v19 =  EngineY::Vertex(p19 , c4  , norm8 , tan3 , bitan3 , t1  , opacity);
             EngineY::Vertex v20 =  EngineY::Vertex(p20 , c4  , norm4 , tan4 , bitan4 , t2  , opacity);
             EngineY::Vertex v21 =  EngineY::Vertex(p21 , c2  , norm3 , tan1 , bitan1 , t3  , opacity);
             EngineY::Vertex v22 =  EngineY::Vertex(p22 , c2  , norm8 , tan2 , bitan2 , t1  , opacity);
             EngineY::Vertex v23 =  EngineY::Vertex(p23 , c3  , norm7 , tan3 , bitan3 , t4  , opacity);
             EngineY::Vertex v24 =  EngineY::Vertex(p24 , c3  , norm3 , tan4 , bitan4 , t3  , opacity);

            // top
             EngineY::Vertex v25 =  EngineY::Vertex(p25 , c1  , norm5 , tan1 , bitan1 , t1  , opacity);
             EngineY::Vertex v26 =  EngineY::Vertex(p26 , c2  , norm1 , tan2 , bitan2 , t2  , opacity);
             EngineY::Vertex v27 =  EngineY::Vertex(p27 , c3  , norm4 , tan3 , bitan3 , t3  , opacity);
             EngineY::Vertex v28 =  EngineY::Vertex(p28 , c4  , norm5 , tan4 , bitan4 , t1  , opacity);
             EngineY::Vertex v29 =  EngineY::Vertex(p29 , c1  , norm8 , tan1 , bitan1 , t4  , opacity);
             EngineY::Vertex v30 =  EngineY::Vertex(p30 , c2  , norm4 , tan2 , bitan2 , t3  , opacity);

            // bottom
             EngineY::Vertex v31 =  EngineY::Vertex(p31 , c3  , norm6 , tan3 , bitan3 , t4  , opacity);
             EngineY::Vertex v32 =  EngineY::Vertex(p32 , c4  , norm2 , tan4 , bitan4 , t3  , opacity);
             EngineY::Vertex v33 =  EngineY::Vertex(p33 , c1  , norm3 , tan1 , bitan1 , t2  , opacity);
             EngineY::Vertex v34 =  EngineY::Vertex(p34 , c2  , norm6 , tan2 , bitan2 , t4  , opacity);
             EngineY::Vertex v35 =  EngineY::Vertex(p35 , c2  , norm7 , tan3 , bitan3 , t1  , opacity);
             EngineY::Vertex v36 =  EngineY::Vertex(p36 , c1  , norm3 , tan4 , bitan4 , t2  , opacity);

            // rect indices
            std::vector<uint32_t> elements{ 0 , 1 , 3 , 1 , 2 , 3 };
    } tvd;

    static const std::vector<EngineY::Vertex> quad_verts {
        tvd.qv1 , tvd.qv2 , tvd.qv3 , tvd.qv4
    };

    static const std::vector<uint32_t> quad_indices {
        0 , 1 , 3 ,
        1 , 2 , 3
    };

    static const std::vector<EngineY::Vertex> cube_verts{
        tvd.v1  , tvd.v2  , tvd.v3  , tvd.v4 ,
        tvd.v5  , tvd.v6  , tvd.v7  , tvd.v8 ,
        tvd.v9  , tvd.v10 , tvd.v11 , tvd.v12 ,
        tvd.v13 , tvd.v14 , tvd.v15 , tvd.v16 ,
        tvd.v17 , tvd.v18 , tvd.v19 , tvd.v20 ,
        tvd.v21 , tvd.v22 , tvd.v23 , tvd.v24 ,
        tvd.v25 , tvd.v26 , tvd.v27 , tvd.v28 ,
        tvd.v29 , tvd.v30 , tvd.v31 , tvd.v32 ,
        tvd.v33 , tvd.v34 , tvd.v35 , tvd.v36
    };

    using VaoData = std::pair<
        std::vector<float> ,
        std::vector<uint32_t>
    >;

    ///> These classes are private cause access to them is restricted to the Resource Manager (think of this 
    ///     as an extension of its private members) and the DUMMYX variables are so we can store the function
    ///     pointers at compile time for faster resource access
    class VertexBuilder {  
        static const std::vector<float> GenerateQuadVerts(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);
        static const std::vector<uint32_t> GetQuadIndices(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);

        static const std::vector<float> GenerateCubeVerts(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);
        static const std::vector<uint32_t> GetCubeIndices(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);

        static const std::vector<float> GenerateSphereVerts(uint32_t stacks , uint32_t slices) { return {}; }
        static const std::vector<uint32_t> GetSphereIndices(uint32_t stacks , uint32_t slices) { return {}; }
        
        ///> If I cache the value of Pi as a constexpr constant somewhere instead of calculating it with
        ///     atan(1) * 4 I can make this function constexpr, since its used for multiple primitives
        static const std::vector<float> BuildIcosahedronVerts();
        static const std::vector<float> GenerateIcosahedronVerts(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);
        static const std::vector<uint32_t> GetIcosahedronIndices(uint32_t DUMMY1 = 0 , uint32_t DUMMY2 = 0);

        static const std::vector<float> GenerateIcosphereVerts(uint32_t subdivisions , uint32_t DUMMY = 0) { return {}; }
        static const std::vector<uint32_t> GetIcosphereIndices(uint32_t subdivisions , uint32_t DUMMY = 0) { return {}; }

        static const std::vector<float> GenerateCubesphereVerts(uint32_t subdivisions , uint32_t DUMMY = 0) { return {}; }
        static const std::vector<uint32_t> GetCubesphereIndices(uint32_t subdivisions , uint32_t DUMMY = 0) { return {}; }
        
        static const std::vector<float> GenerateCylinderVerts(uint32_t stacks , uint32_t slices) { return {}; }
        static const std::vector<uint32_t> GetCylinderIndices(uint32_t stacks , uint32_t slices) { return {}; }
        
        static const std::vector<float> GenerateConeVerts(uint32_t stacks , uint32_t slices) { return {}; }
        static const std::vector<uint32_t> GetConeIndices(uint32_t stacks , uint32_t slices) { return {}; }
        
        static const std::vector<float> GenerateTorusVerts(uint32_t stacks , uint32_t slices) { return {}; }
        static const std::vector<uint32_t> GetTorusIndices(uint32_t stacks , uint32_t slices) { return {}; }
        
        static const std::vector<float> GenerateCapsuleVerts(uint32_t stacks , uint32_t slices) { return {}; }
        static const std::vector<uint32_t> GetCapsuleIndices(uint32_t stacks , uint32_t slices) { return {}; }
        
        static const std::vector<float> GetPrimitiveVertices(UUID id , uint32_t arg1 , uint32_t arg2);
        static const std::vector<uint32_t> GetPrimitiveIndices(UUID id , uint32_t arg1 , uint32_t arg2);
    
        static const VaoData GetPrimitiveData(UUID id , uint32_t arg1 = 0 , uint32_t arg2 = 0) {
            return { GetPrimitiveVertices(id , arg1 , arg2) , GetPrimitiveIndices(id , arg1 , arg2) };
        }

        static constexpr std::array<UUID , 10> primitive_ids = {
            Hash::FNV("quad")        , Hash::FNV("cube")      , Hash::FNV("sphere")     , 
            Hash::FNV("icosahedron") , Hash::FNV("icosphere") , Hash::FNV("cubesphere") ,
            Hash::FNV("cylinder")    , Hash::FNV("cone")    ,
            Hash::FNV("torus")       , Hash::FNV("capsule") ,
        }; 

        static constexpr std::array<
            const std::vector<float>(*)(uint32_t , uint32_t) , 10
        > primitive_vert_builders = {
            &GenerateQuadVerts , &GenerateCubeVerts , &GenerateSphereVerts ,
            &GenerateIcosahedronVerts , &GenerateIcosphereVerts , &GenerateCubesphereVerts ,
            &GenerateCylinderVerts , &GenerateConeVerts ,
            &GenerateTorusVerts , &GenerateCapsuleVerts
        };
        
        static constexpr std::array<
            const std::vector<uint32_t>(*)(uint32_t , uint32_t) , 10
        > primitive_index_builders = {
            &GetQuadIndices , &GetCubeIndices , &GetSphereIndices ,
            &GetIcosahedronIndices , &GetIcosphereIndices , &GetCubesphereIndices ,
            &GetCylinderIndices , &GetConeIndices ,
            &GetTorusIndices , &GetCapsuleIndices
        };

        friend class ResourceHandler;
    };

} // namespace primitives

} // namespace YE

#endif // YE_PRIMITIVE_VAO_DATA_HPP
