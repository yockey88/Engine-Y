#include "rendering/framebuffer.hpp"

#include "log.hpp"
#include "engine.hpp"

namespace YE {

    Framebuffer::~Framebuffer() {
        glDeleteTextures(1 , &color_attachment);
        glDeleteRenderbuffers(1 , &rbo);
        glDeleteFramebuffers(1 , &fbo);

        ydelete vao;
    }

    void Framebuffer::Create() {
        complete = true;

        vao = ynew VertexArray(vertices , indices , layout);
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

    void Framebuffer::Draw(DrawMode mode) {
        if (!complete) {
            ENGINE_ERROR("Rendering Invalid Framebuffer");
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D , color_attachment);
            if (shader != nullptr) {
                shader->Bind();
                shader->SetUniformInt("screen_tex" , 0);
                shader->SetUniformMat4("model" , model);
            }
            vao->Draw(mode);
            if (shader != nullptr)
                shader->Unbind();
            glBindTexture(GL_TEXTURE_2D , 0);
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

}