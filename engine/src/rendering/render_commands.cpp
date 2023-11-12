#include "rendering/render_commands.hpp"

#include "log.hpp"
#include "core/resource_handler.hpp"
#include "core/font.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/model.hpp"
#include "rendering/camera.hpp"
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
    
    void DrawFont::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (shader == nullptr) {
            ENGINE_WARN("Failed to execute DrawFont :: Shader is null");
            return;
        }
        
        const auto& font_geometry = font->GetGeometryData().font_geometry;
        const auto& metrics = font_geometry.getMetrics();

        Texture* font_atlas = font->GetAtlasTexture();
        if (font_atlas == nullptr) {
            ENGINE_WARN("Failed to execute DrawFont :: Font atlas is null");
            return;
        }

        const glm::ivec2& atlas_size = font_atlas->Size();

        /// temporary until background color for text is implemented (i.e rendering text onto a wall or other surface)
        const glm::ivec4& bgcolor = Renderer::Instance()->ActiveWindow()->ClearColor();
        /// end temporary

        double x = 0.f;
        double fscale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.f;

        for (size_t i = 0; i < text.size(); ++i) {
            char c = text[i];

            const msdf_atlas::GlyphGeometry* glyph = font_geometry.getGlyph(c);
            if (glyph == nullptr) {
                glyph = font_geometry.getGlyph('?');
            } 

            if (glyph == nullptr) {
                LOG_WARN("Rendering corrupter font atlas :: [{}]" , font->Path().string());
                return;
            }

            // double al , ab , ar , at;
            // glyph->getQuadPlaneBounds(al , ab , ar , at);
            // glm::vec2 tex_min((float)al , (float)ab);
            // glm::vec2 tex_max((float)ar , (float)at);

            double pl , pb , pr , pt;
            glyph->getQuadPlaneBounds(pl , pb , pr , pt);
            glm::vec2 pos_min((float)pl , (float)pb);
            glm::vec2 pos_max((float)pr , (float)pt);

            float texel_width = 1.f / font_atlas->Size().x;
            float texel_height = 1.f / font_atlas->Size().y;
            glm::vec2 tex_min = pos_min * glm::vec2(texel_width , texel_height);
            glm::vec2 tex_max = pos_max * glm::vec2(texel_width , texel_height);

            pos_min *= fscale , pos_max *= fscale;
            pos_min += glm::vec2(x , y) , pos_max += glm::vec2(x , y);

            glm::vec4 pos1 = model * glm::vec4(pos_min , 0.f , 1.f);
            glm::vec4 pos2 = model * glm::vec4(pos_min.x , pos_max.y , 0.f , 1.f);
            glm::vec4 pos3 = model * glm::vec4(pos_max , 0.f , 1.f);
            glm::vec4 pos4 = model * glm::vec4(pos_max.x , pos_min.y , 0.f , 1.f);

            std::vector<float> vertices = {
                pos1.x , pos1.y , 0.f , tex_min.x , tex_min.y ,
                pos2.x , pos2.y , 0.f , tex_min.x , tex_max.y ,
                pos3.x , pos3.y , 0.f , tex_max.x , tex_max.y ,
                pos4.x , pos4.y , 0.f , tex_max.x , tex_min.y
            };

            std::vector<uint32_t> indices = {
                0 , 1 , 3 , 
                1 , 2 , 3
            };

            vao = ynew VertexArray(vertices , indices , { 3 , 2 });
            vao->Upload();

            // Render here
            if (vao->Valid()) {
                shader->Bind();
                SetCameraUniforms(shader , camera);
                shader->SetUniformMat4("model" , model);
                shader->SetUniformVec4("bgcolor" , bgcolor);
                shader->SetUniformVec4("fgcolor" , glm::vec4(1.f));
                shader->SetUniformInt("msdf" , 0);
                shader->SetUniformFloat("px_range" , 2);
                font_atlas->Bind();
                vao->Draw(mode);
                font_atlas->Unbind();
                shader->Unbind();
            }

            if (i < text.size() - 1) {
                double advance = glyph->getAdvance();
                char next = text[i + 1];
                font_geometry.getAdvance(advance , c , next);

                float kerning_offset = 0.f;
                x += advance * fscale + kerning_offset;
            }

            ydelete vao;

            // Advance to next glyph
        }
    }

    void DrawRenderable::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (renderable.vao == nullptr) {
            ENGINE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }
        
        if (renderable.shader == nullptr)
            renderable.shader = ResourceHandler::Instance()->GetShader(renderable.shader_name);
        if (renderable.shader == nullptr) {
            renderable.shader = ResourceHandler::Instance()->GetCoreShader(renderable.shader_name);

            if (renderable.shader == nullptr) {
                ENGINE_WARN("Failed to execute DrawTexturedVao :: Shader [{0}] is null" , renderable.shader_name);
                renderable.corrupted = true;
                return;
            }
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
            ENGINE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }

        for (uint32_t i = 0; i < renderable.textures.size(); ++i)
            if (renderable.textures[i] == nullptr) {
                ENGINE_WARN("Failed to execute DrawTexturedVao :: Texture [{0}] is null" , i);
                renderable.corrupted = true;
                return;
            }
        
        if (renderable.shader == nullptr)
            renderable.shader = ResourceHandler::Instance()->GetShader(renderable.shader_name);
        if (renderable.shader == nullptr) {
            renderable.shader = ResourceHandler::Instance()->GetCoreShader(renderable.shader_name);

            if (renderable.shader == nullptr) {
                ENGINE_WARN("Failed to execute DrawTexturedVao :: Shader [{0}] is null" , renderable.shader_name);
                renderable.corrupted = true;
                return;
            }
        }

        YE_CRITICAL_ASSERTION(renderable.shader != nullptr , "Shader is null | UNDEFINED BEHAVIOUR");

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
            ENGINE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }
        
        if (renderable.shader == nullptr)
            renderable.shader = ResourceHandler::Instance()->GetShader(renderable.shader_name);
        if (renderable.shader == nullptr) {
            renderable.shader = ResourceHandler::Instance()->GetCoreShader(renderable.shader_name);

            if (renderable.shader == nullptr) {
                ENGINE_WARN("Failed to execute DrawTexturedVao :: Shader [{0}] is null" , renderable.shader_name);
                renderable.corrupted = true;
                return;
            }
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
            ENGINE_WARN("Failed to execute DrawTexturedVao :: VAO is null");
            renderable.corrupted = true;
            return;
        }

        if (light == nullptr) {
            ENGINE_WARN("Failed to execute DrawPointLight :: Light is null");
            return;
        }
        
        if (renderable.shader == nullptr)
            renderable.shader = ResourceHandler::Instance()->GetShader(renderable.shader_name);
        if (renderable.shader == nullptr) {
            renderable.shader = ResourceHandler::Instance()->GetCoreShader(renderable.shader_name);

            if (renderable.shader == nullptr) {
                ENGINE_WARN("Failed to execute DrawTexturedVao :: Shader [{0}] is null" , renderable.shader_name);
                renderable.corrupted = true;
                return;
            }
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