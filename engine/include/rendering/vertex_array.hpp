#ifndef YE_VERTEX_ARRAY_HPP
#define YE_VERTEX_ARRAY_HPP

#include <vector>
#include <string>

#include <glad/glad.h>

#include "vertex.hpp"

namespace YE {

    enum BufferType {
        STATIC = GL_STATIC_DRAW ,
        DYNAMIC = GL_DYNAMIC_DRAW
    };

    enum DrawMode {
        TRIANGLES = GL_TRIANGLES ,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP ,
        TRIANGLE_FAN = GL_TRIANGLE_FAN ,
        LINES = GL_LINES ,
        LINE_STRIP = GL_LINE_STRIP ,
        LINE_LOOP = GL_LINE_LOOP ,
        POINTS = GL_POINTS
    };

    class VertexArray;

    struct VertexArrayResource {
        VertexArray* vao = nullptr;
        std::string name;
    };

    class VertexArray {

        uint32_t VAO = 0;
        uint32_t VBO = 0;
        uint32_t EBO = 0;
        BufferType buffer_type;

        uint32_t stride = 0;

        bool has_indices = false;
        bool valid = false;

        bool yverts = false;

        std::vector<Vertex> yvertices;
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        std::vector<uint32_t> layout;

        void LoadVertices();
        
        VertexArray(VertexArray&&) = delete;
        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        public:
            VertexArray(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices , BufferType buffer_type = BufferType::STATIC) 
                        : yvertices(vertices) , indices(indices) , layout(Vertex::Layout()) , buffer_type(buffer_type) , yverts(true) {}
            VertexArray(const std::vector<float>& vertices, const std::vector<uint32_t>& indices , const std::vector<uint32_t>& layout ,
                        BufferType buffer_type = BufferType::STATIC) 
                        : vertices(vertices) , indices(indices) , layout(layout) , buffer_type(buffer_type) , yverts(false) {}
            ~VertexArray();

            void Upload();
            void Draw(DrawMode mode) const;

            inline bool Valid() const { return valid; }
    };

}

#endif // !YE_VERTEX_ARRAY_HPP