#include "rendering/render_commands.hpp"

#include "log.hpp"
#include "core/resource_handler.hpp"
#include "scene/components.hpp"

namespace YE {
    
    void RenderCommand::SetCameraUniforms(Shader* shader , Camera* camera) {
        if (camera != nullptr) {
            shader->SetUniformInt("camera_active" , 1);
            shader->SetUniformMat4("view" , camera->View());
            shader->SetUniformMat4("proj" , camera->Projection());
            shader->SetUniformVec3("view_pos" , camera->Position());
        } else {
            shader->SetUniformInt("camera_active" , 0);
        }
    }
    
    void DrawVao::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (vao == nullptr) {
            YE_WARN("Failed to execute DrawVao :: VAO is null");
            return;
        }

        if (vao->Valid()) {
            shader->Bind();
            SetCameraUniforms(shader , camera);
            shader->SetUniformMat4("model" , model);
            vao->Draw(mode);
            shader->Unbind();
        }

        ydelete vao;
    }

    void DrawRenderable::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (renderable.vao == nullptr) {
            YE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }
        
        if (renderable.vao->Valid()) {
            renderable.shader->Bind();
            SetCameraUniforms(renderable.shader , camera);
            renderable.shader->SetUniformMat4("model" , model);
            renderable.vao->Draw(mode);
            renderable.shader->Unbind();
        }
    }

    void DrawTexturedRenderable::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (renderable.vao == nullptr) {
            YE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }

        for (uint32_t i = 0; i < renderable.textures.size(); ++i)
            if (renderable.textures[i] == nullptr) {
                YE_WARN("Failed to execute DrawTexturedVao :: Texture [{0}] is null" , i);
                renderable.corrupted = true;
                return;
            }

        if (renderable.vao->Valid()) {
            renderable.shader->Bind();
            for (uint32_t i = 0; i < renderable.textures.size(); ++i) {
                renderable.shader->SetUniformInt("tex" + std::to_string(i) , i);
                if (renderable.textures[i] != nullptr) renderable.textures[i]->Bind(i);
            }
            SetCameraUniforms(renderable.shader , camera);
            renderable.shader->SetUniformMat4("model" , model);
            renderable.vao->Draw(mode);
            renderable.shader->Unbind();
            for (uint32_t i = 0; i < renderable.textures.size(); ++i)
                if (renderable.textures[i] != nullptr) renderable.textures[i]->Unbind(i);
        }
    }
    
    void DrawRenderableModel::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (renderable.model == nullptr) {
            YE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }

        if (renderable.model->Valid()) {
            renderable.shader->Bind();
            SetCameraUniforms(renderable.shader , camera);
            renderable.shader->SetUniformMat4("model" , model_matrix);
            renderable.model->Draw(renderable.shader , mode);
            renderable.shader->Unbind();
        }
    }
    
    void DrawPointLight::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (renderable.vao == nullptr) {
            YE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }

        if (light == nullptr) {
            YE_WARN("Failed to execute DrawPointLight :: Light is null");
            return;
        }

        if (renderable.vao->Valid()) {
            renderable.shader->Bind();
            SetCameraUniforms(renderable.shader , camera);
            renderable.shader->SetUniformMat4("model" , model_matrix);
            renderable.shader->SetUniformVec3("light_color" , light->diffuse);
            renderable.vao->Draw(mode);
            renderable.shader->Unbind();
        }
    }

}