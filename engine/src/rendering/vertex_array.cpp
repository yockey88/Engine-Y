#include "rendering/vertex_array.hpp"

#include <vector>
#include <stacktrace>

#include <glm/glm.hpp>

#include "core/log.hpp"
#include "rendering/gl_error_helper.hpp"

namespace EngineY {

    void VertexArray::Generate() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }
    
    void VertexArray::LoadVertices() {
        for (auto& v : yvertices) {
            vertices.push_back(v.position.x);
            vertices.push_back(v.position.y);
            vertices.push_back(v.position.z);

            vertices.push_back(v.color.x);
            vertices.push_back(v.color.y);
            vertices.push_back(v.color.z);

            vertices.push_back(v.normal.x);
            vertices.push_back(v.normal.y);
            vertices.push_back(v.normal.z);

            vertices.push_back(v.tangent.x);
            vertices.push_back(v.tangent.y);
            vertices.push_back(v.tangent.z);

            vertices.push_back(v.bitangent.x);
            vertices.push_back(v.bitangent.y);
            vertices.push_back(v.bitangent.z);

            vertices.push_back(v.texcoord.x);
            vertices.push_back(v.texcoord.y);

            vertices.push_back(v.opacity);
        }
    }
    
    void VertexArray::BufferData(uint32_t buffer , size_t size , const void* data) {
        GLenum buffer_enum;
        if (buffer == VBO) {
            buffer_enum = GL_ARRAY_BUFFER;
        } else if (buffer == EBO) {
            buffer_enum = GL_ELEMENT_ARRAY_BUFFER;
        } else {
            ENGINE_ERROR("Vertex Array buffering data to invalid buffer");
            return;
        }

        glBindBuffer(buffer_enum , buffer);
        glBufferData(buffer_enum , size , data , buffer_type);
    }
    
    uint32_t VertexArray::SetLayout() {
        uint32_t stride = 0;
        if (yverts) {
            LoadVertices(); 
            stride = Vertex::Size();
        } else {
            for (auto& l : layout)
                stride += l;
            stride *= sizeof(uint32_t);
        }

        return stride;
    }
    
    void VertexArray::SetAttributes(uint32_t stride) {
        uint32_t offset = 0;
        for (size_t i = 0; i < layout.size(); ++i) {
            glVertexAttribPointer(i , layout[i] , GL_FLOAT, GL_FALSE , stride , (void*)(offset * sizeof(float))); 
            glEnableVertexAttribArray(i);

            offset += layout[i];
        }
    }
    
    void VertexArray::Upload() {
        if (buffer_type == BufferType::DYNAMIC) {
            ENGINE_ERROR("Uploading static data to dynmaic vertex array");
            return;
        }

        if (indices.size() > 0) {
            has_indices = true;
        }

        uint32_t stride = SetLayout();
        
        Bind();
        
        BufferData(VBO , sizeof(float) * vertices.size() , vertices.data());
        if (has_indices) {
            BufferData(EBO , sizeof(uint32_t) * indices.size() , indices.data());
        }

        SetAttributes(stride);

        glBindBuffer(GL_ARRAY_BUFFER , 0);
        Unbind();

        valid = true;
    }

    VertexArray::VertexArray(uint32_t size) : buffer_type(BufferType::DYNAMIC) {
        Generate();

        Bind();
        BufferData(VBO , size , nullptr);
    }

    VertexArray::VertexArray(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) 
            : yvertices(vertices) , indices(indices) , layout(Vertex::Layout()) , 
            buffer_type(BufferType::STATIC) , yverts(true) {
        Generate();
        Upload();
    }

    VertexArray::VertexArray(
        const std::vector<float>& vertices, const std::vector<uint32_t>& indices , 
        const std::vector<uint32_t>& layout 
    ) : vertices(vertices) , indices(indices) , layout(layout) , 
            buffer_type(BufferType::STATIC) , yverts(false) {
        Generate();
        Upload(); 
    }
    
    VertexArray::~VertexArray() {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void VertexArray::Bind() const {
        glBindVertexArray(VAO);
    }

    void VertexArray::SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        if (buffer_type != BufferType::DYNAMIC) {
            ENGINE_ERROR("Uploading dynamic data to static vertex array");
            return;
        }

        yvertices = vertices;
        yverts = true;

        this->indices = indices;
        this->layout = Vertex::Layout();
        
        if (indices.size() > 0) {
            has_indices = true;
        }

        uint32_t stride = SetLayout();
        
        Bind();
        
        BufferData(VBO , sizeof(float) * vertices.size() , vertices.data());
        if (has_indices) {
            BufferData(EBO , sizeof(uint32_t) * indices.size() , indices.data());
        }

        SetAttributes(stride);

        glBindBuffer(GL_ARRAY_BUFFER , 0);
        Unbind();

        valid = true;
    }
    
    void VertexArray::SetData(
        const std::vector<float>& vertices , const std::vector<uint32_t>& indices ,
        const std::vector<uint32_t>& layout
    ) {
        if (buffer_type != BufferType::DYNAMIC) {
            ENGINE_ERROR("Uploading dynamic data to static vertex array");
            return;
        }
        
        this->vertices = vertices;
        this->indices = indices;
        this->layout = layout;

        if (indices.size() > 0) {
            has_indices = true;
        }

        uint32_t stride = SetLayout();
        
        Bind();
        
        BufferData(VBO , sizeof(float) * vertices.size() , vertices.data());
        if (has_indices) {
            BufferData(EBO , sizeof(uint32_t) * indices.size() , indices.data());
        }

        SetAttributes(stride);

        glBindBuffer(GL_ARRAY_BUFFER , 0);
        Unbind();

        valid = true;
    }

    void VertexArray::Draw(DrawMode mode) const {
        if (!valid) {
            ENGINE_ERROR("Rendering Invalid Vertex Array");
        } else {
            Bind();
            if (has_indices) {
                glDrawElements(mode , indices.size() , GL_UNSIGNED_INT , (void*)0);
            } else {
                glDrawArrays(mode , 0 , vertices.size());
            }
            Unbind();
        }
    }

    void VertexArray::Unbind() const {
        glBindVertexArray(0);
    }

}
