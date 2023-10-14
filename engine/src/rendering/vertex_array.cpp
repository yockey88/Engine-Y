#include "rendering/vertex_array.hpp"

#include <vector>

#include <glm/glm.hpp>

#include "log.hpp"
#include "rendering/gl_error_helper.hpp"

namespace YE {

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

        stride = Vertex::Size();
    }

    VertexArray::~VertexArray() {
        if (valid) {
            if (has_indices) {
                glDeleteBuffers(1, &EBO);
            }
            glDeleteBuffers(1, &VBO);
            glDeleteVertexArrays(1, &VAO);
        }
    }

    void VertexArray::Upload() {
        if (indices.size() > 0) {
            has_indices = true;
        }

        if (yverts) {
            LoadVertices();
        } else {
            for (auto& l : layout)
                stride += l;
            stride *= sizeof(uint32_t);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (has_indices) glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER , VBO);
        glBufferData(GL_ARRAY_BUFFER , sizeof(float) * vertices.size() , vertices.data() , buffer_type);

        if (has_indices) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER , sizeof(uint32_t) * indices.size() , indices.data() , buffer_type);
        }

        uint32_t offset = 0;
        for (size_t i = 0; i < layout.size(); ++i) {
            glVertexAttribPointer(i , layout[i] , GL_FLOAT, GL_FALSE , stride , (void*)(offset * sizeof(float))); 
            glEnableVertexAttribArray(i);

            offset += layout[i];
        }

        glBindBuffer(GL_ARRAY_BUFFER , 0);
        glBindVertexArray(0);

        valid = true;
    }
    
    void VertexArray::Draw(DrawMode mode) const {
        if (!valid) {
            YE_ERROR("Rendering Invalid Vertex Array");
        } else {
            glBindVertexArray(VAO);

            if (has_indices) {
                glDrawElements(mode , indices.size() , GL_UNSIGNED_INT , (void*)0);
            } else {
                glDrawArrays(mode , 0 , vertices.size());
            }

            glBindVertexArray(0);
        }
    }

}