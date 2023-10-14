#ifndef YE_PRIMITIVE_VAO_DATA_HPP
#define YE_PRIMITIVE_VAO_DATA_HPP

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "rendering/vertex.hpp"

namespace YE {

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
            YE::Vertex qv1 = YE::Vertex(qp1 , c1  , rnorm1 , tan1 , bitan1 , t1  , opacity);
            YE::Vertex qv2 = YE::Vertex(qp2 , c2  , rnorm2 , tan2 , bitan2 , t2  , opacity);
            YE::Vertex qv3 = YE::Vertex(qp3 , c3  , rnorm3 , tan3 , bitan3 , t3  , opacity);
            YE::Vertex qv4 = YE::Vertex(qp4 , c4  , rnorm4 , tan4 , bitan4 , t4  , opacity);

            // top
            YE::Vertex  v1 = YE::Vertex(p1 , c1  , norm1 , tan1 , bitan1 , t1  , opacity);
            YE::Vertex  v2 = YE::Vertex(p2 , c2  , norm2 , tan2 , bitan2 , t2  , opacity);
            YE::Vertex  v3 = YE::Vertex(p3 , c3  , norm3 , tan3 , bitan3 , t3  , opacity);
            YE::Vertex  v4 = YE::Vertex(p4 , c4  , norm1 , tan4 , bitan4 , t1  , opacity);
            YE::Vertex  v5 = YE::Vertex(p5 , c1  , norm4 , tan1 , bitan1 , t4  , opacity);
            YE::Vertex  v6 = YE::Vertex(p6 , c2  , norm3 , tan2 , bitan2 , t3  , opacity);

            // bottom
            YE::Vertex  v7 = YE::Vertex(p7  , c3  , norm5 , tan3 , bitan3 , t1  , opacity);
            YE::Vertex  v8 = YE::Vertex(p8  , c4  , norm6 , tan4 , bitan4 , t2  , opacity);
            YE::Vertex  v9 = YE::Vertex(p9  , c1  , norm7 , tan1 , bitan1 , t3  , opacity);
            YE::Vertex v10 = YE::Vertex(p10 , c2  , norm5 , tan2 , bitan2 , t1  , opacity);
            YE::Vertex v11 = YE::Vertex(p11 , c2  , norm8 , tan3 , bitan3 , t4  , opacity);
            YE::Vertex v12 = YE::Vertex(p12 , c1  , norm7 , tan4 , bitan4 , t3  , opacity);

            // left 
            YE::Vertex v13 = YE::Vertex(p13 , c4  , norm5 , tan1 , bitan1 , t4  , opacity);
            YE::Vertex v14 = YE::Vertex(p14 , c3  , norm1 , tan2 , bitan2 , t3  , opacity);
            YE::Vertex v15 = YE::Vertex(p15 , c3  , norm2 , tan3 , bitan3 , t2  , opacity);
            YE::Vertex v16 = YE::Vertex(p16 , c4  , norm5 , tan4 , bitan4 , t4  , opacity);
            YE::Vertex v17 = YE::Vertex(p17 , c1  , norm6 , tan1 , bitan1 , t1  , opacity);
            YE::Vertex v18 = YE::Vertex(p18 , c1  , norm2 , tan2 , bitan2 , t2  , opacity);

            // right
            YE::Vertex v19 = YE::Vertex(p19 , c4  , norm8 , tan3 , bitan3 , t1  , opacity);
            YE::Vertex v20 = YE::Vertex(p20 , c4  , norm4 , tan4 , bitan4 , t2  , opacity);
            YE::Vertex v21 = YE::Vertex(p21 , c2  , norm3 , tan1 , bitan1 , t3  , opacity);
            YE::Vertex v22 = YE::Vertex(p22 , c2  , norm8 , tan2 , bitan2 , t1  , opacity);
            YE::Vertex v23 = YE::Vertex(p23 , c3  , norm7 , tan3 , bitan3 , t4  , opacity);
            YE::Vertex v24 = YE::Vertex(p24 , c3  , norm3 , tan4 , bitan4 , t3  , opacity);

            // top
            YE::Vertex v25 = YE::Vertex(p25 , c1  , norm5 , tan1 , bitan1 , t1  , opacity);
            YE::Vertex v26 = YE::Vertex(p26 , c2  , norm1 , tan2 , bitan2 , t2  , opacity);
            YE::Vertex v27 = YE::Vertex(p27 , c3  , norm4 , tan3 , bitan3 , t3  , opacity);
            YE::Vertex v28 = YE::Vertex(p28 , c4  , norm5 , tan4 , bitan4 , t1  , opacity);
            YE::Vertex v29 = YE::Vertex(p29 , c1  , norm8 , tan1 , bitan1 , t4  , opacity);
            YE::Vertex v30 = YE::Vertex(p30 , c2  , norm4 , tan2 , bitan2 , t3  , opacity);

            // bottom
            YE::Vertex v31 = YE::Vertex(p31 , c3  , norm6 , tan3 , bitan3 , t4  , opacity);
            YE::Vertex v32 = YE::Vertex(p32 , c4  , norm2 , tan4 , bitan4 , t3  , opacity);
            YE::Vertex v33 = YE::Vertex(p33 , c1  , norm3 , tan1 , bitan1 , t2  , opacity);
            YE::Vertex v34 = YE::Vertex(p34 , c2  , norm6 , tan2 , bitan2 , t4  , opacity);
            YE::Vertex v35 = YE::Vertex(p35 , c2  , norm7 , tan3 , bitan3 , t1  , opacity);
            YE::Vertex v36 = YE::Vertex(p36 , c1  , norm3 , tan4 , bitan4 , t2  , opacity);

            // rect indices
            std::vector<uint32_t> elements{ 0 , 1 , 3 , 1 , 2 , 3 };
    } tvd;

    static const std::vector<YE::Vertex> quad_verts {
        tvd.qv1 , tvd.qv2 , tvd.qv3 , tvd.qv4
    };

    static const std::vector<uint32_t> quad_indices {
        0 , 1 , 3 ,
        1 , 2 , 3
    };

    static const std::vector<YE::Vertex> cube_verts{
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

    // static const std::vector<YE::Vertex> cube_verts2{
    //     tvd.tv1 , tvd.tv2 , tvd.tv3 , tvd.tv4 ,
    //     tvd.tv5 , tvd.tv6 , tvd.tv7 , tvd.tv8
    // };

    // static const std::vector<uint32_t> cube_indices {        
    //     //Top
    //     2, 6, 7, // 3 , 7 , 8
    //     2, 3, 7, // 3 , 4 , 8

    //     //Bottom
    //     0, 4, 5, // 1 , 5 , 6
    //     0, 1, 5, // 1 , 2 , 6

    //     //Left
    //     0, 2, 6, // 1 , 3 , 7
    //     0, 4, 6, // 1 , 5 , 7

    //     //Right
    //     1, 3, 7, // 2 , 4 , 8
    //     1, 5, 7, // 2 , 6 , 8

    //     //Front
    //     0, 2, 3, // 1 , 3 , 4
    //     0, 1, 3, // 1 , 2 , 4

    //     //Back
    //     4, 6, 7, // 5 , 7 , 8
    //     4, 5, 7  // 5 , 6 , 8
    // };

} // namespace primitives

} // namespace YE

#endif // YE_PRIMITIVE_VAO_DATA_HPP