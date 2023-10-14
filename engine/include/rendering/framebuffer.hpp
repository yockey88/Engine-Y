#ifndef YE_FRAMEBUFFER_HPP
#define YE_FRAMEBUFFER_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/vertex.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"

namespace YE {
    
    enum BufferBit : int32_t {
        COLOR_BUFFER = GL_COLOR_BUFFER_BIT ,
        DEPTH_BUFFER = GL_DEPTH_BUFFER_BIT ,
        STENCIL_BUFFER = GL_STENCIL_BUFFER_BIT
    };

    class Framebuffer {

        Shader* shader = nullptr;
        VertexArray* vao = nullptr;
        int32_t buffer_type = 0;
        std::vector<BufferBit> buffer_bits;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        std::vector<uint32_t> layout;

        glm::mat4 model;
        glm::vec4 clear_color;
        glm::ivec2 size;

        uint32_t fbo = 0;
        uint32_t rbo = 0;
        uint32_t framebuffer = 0;

        uint32_t intermediate_fbo = 0;
        uint32_t screen_texture = 0;

        uint32_t color_attachment = 0;

        bool complete = false;

        Framebuffer(Framebuffer&&) = delete;
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        friend class Window;

        public:
            Framebuffer(const std::vector<float>& vertices , const std::vector<uint32_t> indices ,
                        const std::vector<uint32_t>& layout , const glm::ivec2& size)
                : vertices(vertices) , indices(indices) , layout(layout) , size(size) {}
            ~Framebuffer();

            void Create();
            void Draw(DrawMode mode = DrawMode::TRIANGLES);

            void SetBufferType(const std::vector<BufferBit>& bits);

            void HandleResize(const glm::ivec2& size);

            void BindFrame() const;
            void UnbindFrame() const;

            inline const glm::ivec2& Size() const { return size; }
            inline bool Valid() const { return complete; }
            inline void AttachShader(Shader* shader) { this->shader = shader; }
            inline void SetClearColor(const glm::vec4& color) { clear_color = color; }
    };

}

#endif // !YE_FRAMEBUFFER_HPP