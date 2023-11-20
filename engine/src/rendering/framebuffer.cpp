#include "rendering/framebuffer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.hpp"
#include "core/log.hpp"
#include "core/memory/memory_manager.hpp"
#include "rendering/renderer.hpp"

namespace EngineY {

    void Framebuffer::Create() {
        complete = true;

        vao = ynew(VertexArray ,vertices , indices , layout);
        model = glm::mat4(1.0f);
        clear_color = glm::vec4(0.1f , 0.1f , 0.1f , 1.0f);

        glGenFramebuffers(1 , &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER , fbo);

        glGenTextures(1, &color_attachment);
        glBindTexture(GL_TEXTURE_2D , color_attachment);
        glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , size.x , size.y , 0 , GL_RGB , GL_UNSIGNED_BYTE , nullptr);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR); 
        glBindTexture(GL_TEXTURE_2D , 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , color_attachment , 0);

        glGenRenderbuffers(1 , &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER , rbo);
        glRenderbufferStorage(GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , size.x , size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ENGINE_ERROR("Framebuffer is not complete!");
            complete = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER , 0);

        SetBufferType({ BufferBit::COLOR_BUFFER , BufferBit::DEPTH_BUFFER , BufferBit::STENCIL_BUFFER });

        glGenFramebuffers(1 , &intermediate_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER , intermediate_fbo);
        
        glGenTextures(1 , &screen_texture);
        glBindTexture(GL_TEXTURE_2D , screen_texture);

        glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , size.x , size.y , 0 , GL_RGB , GL_UNSIGNED_BYTE , nullptr);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , screen_texture , 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ENGINE_ERROR("Intermediate Framebuffer is not complete!");
            complete = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
    }
    
    Framebuffer::Framebuffer(
        const std::vector<float>& vertices , const std::vector<uint32_t> indices ,
        const std::vector<uint32_t>& layout , const glm::ivec2& size
    ) : vertices(vertices) , indices(indices) , layout(layout) , size(size) {
        Create();
    }

    void Framebuffer::Draw(DrawMode mode) {
        if (!complete) {
            ENGINE_ERROR("Rendering Invalid Framebuffer");
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D , color_attachment);
            
            if (shader != nullptr) {
                glm::ivec2 win_size = Renderer::Instance()->ActiveWindow()->GetSize();
                glm::ivec2 scale = size / glm::ivec2{ win_size.x , win_size.y };
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::scale(model , glm::vec3(scale , 1.0f));
            
                shader->Bind();
                shader->SetUniformInt("screen_tex" , 0);
                shader->SetUniformMat4("model" , model);
            } else {
                ENGINE_FERROR("Framebuffer shader is nullptr");
            }
            vao->Draw(mode);
        }
    }

    void Framebuffer::SetBufferType(const std::vector<BufferBit>& bits) {
        buffer_bits = bits;
        for (auto bit : bits)
            buffer_type |= bit;
    }

    void Framebuffer::HandleResize(const glm::ivec2& size) {
        this->size = size;

        glDeleteTextures(1 , &color_attachment);
        glDeleteRenderbuffers(1 , &rbo);
        glDeleteFramebuffers(1 , &fbo);
        glDeleteFramebuffers(1 , &intermediate_fbo);
        glDeleteTextures(1 , &screen_texture);
        
        glGenFramebuffers(1 , &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER , fbo);

        glGenTextures(1, &color_attachment);
        glBindTexture(GL_TEXTURE_2D , color_attachment);
        glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , size.x , size.y , 0 , GL_RGB , GL_UNSIGNED_BYTE , nullptr);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D , 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , color_attachment , 0);

        glGenRenderbuffers(1 , &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER , rbo);
        glRenderbufferStorage(GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , size.x , size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ENGINE_ERROR("Framebuffer is not complete | Could not handle resize");
            complete = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
    }

    void Framebuffer::BindFrame() const {
        glBindFramebuffer(GL_FRAMEBUFFER , fbo);
        glViewport(0 , 0 , static_cast<uint32_t>(size.x) , static_cast<uint32_t>(size.y));
        glClearColor(clear_color.x , clear_color.y , clear_color.z , clear_color.w);
        glClear(buffer_type);
        glEnable(GL_DEPTH_TEST);
    }

    void Framebuffer::UnbindFrame() const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER , fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER , intermediate_fbo);
        glBlitFramebuffer(0 , 0 , size.x , size.y , 0 , 0 , size.x , size.y , buffer_type , GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
        glClearColor(clear_color.x , clear_color.y , clear_color.z , clear_color.w);
        glClear(buffer_type);
        glDisable(GL_DEPTH_TEST);
    }
    
    void Framebuffer::Destroy() {
        glDeleteTextures(1 , &color_attachment);
        glDeleteRenderbuffers(1 , &rbo);
        glDeleteFramebuffers(1 , &fbo);
        glDeleteFramebuffers(1 , &intermediate_fbo);
        glDeleteTextures(1 , &screen_texture);

        ydelete(vao);
    }

}
