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

        bool has_indices = false;
        bool valid = false;

        bool yverts = false;

        std::vector<Vertex> yvertices;
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        std::vector<uint32_t> layout;

        void Generate();
        void LoadVertices();
        void BufferData(uint32_t buffer , size_t size , const void* data);
        uint32_t SetLayout();
        void SetAttributes(uint32_t stride);
        void Upload();
        
        VertexArray(VertexArray&&) = delete;
        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        public:
            VertexArray(uint32_t size); 
            VertexArray(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices); 
            VertexArray(
                const std::vector<float>& vertices, const std::vector<uint32_t>& indices , 
                const std::vector<uint32_t>& layout 
            ); 
            ~VertexArray();

            void Bind() const;
            void SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
            void SetData(
                const std::vector<float>& vertices , const std::vector<uint32_t>& indices ,
                const std::vector<uint32_t>& layout
            );
            void Draw(DrawMode mode) const;
            void Unbind() const;

            inline bool Valid() const { return valid; }
    };

}

#endif // !YE_VERTEX_ARRAY_HPP