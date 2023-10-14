#ifndef YE_VERTEX_HPP
#define YE_VERTEX_HPP

#include <vector>

#include <glm/glm.hpp>

namespace YE {

    struct Vertex {
        glm::vec3 position{ 0.f , 0.f , 0.f };
        glm::vec3 color{ 1.f , 1.f , 1.f };
        glm::vec3 normal{ 1.f , 1.f , 1.f };
        glm::vec3 tangent{ 1.f , 1.f , 1.f };
        glm::vec3 bitangent{ 1.f , 1.f , 1.f };
        glm::vec2 texcoord{ 0.f , 0.f };
        float opacity = 1.0f;

        inline const static std::vector<uint32_t> layout{ 3 , 3 , 3 , 3 , 3 , 2 , 1 };

        static const size_t Size() { return sizeof(Vertex); }
        static const size_t PositionOffset() { return offsetof(Vertex , position); }
        static const size_t ColorOffset() { return offsetof(Vertex , color); }
        static const size_t NormalOffset() { return offsetof(Vertex , normal); }
        static const size_t TangentOffset() { return offsetof(Vertex , tangent); }
        static const size_t BitangentOffset() { return offsetof(Vertex , bitangent); }
        static const size_t TexcoordOffset() { return offsetof(Vertex , texcoord); }
        static const size_t OpacityOffset() { return offsetof(Vertex , opacity); }
        static const std::vector<uint32_t>& Layout() { return layout; }

        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && 
                   normal == other.normal && tangent == other.tangent && 
                   bitangent == other.bitangent && texcoord == other.texcoord 
                   && opacity == other.opacity;
        }
    };

}

#endif // !YE_VERTEX_HPP