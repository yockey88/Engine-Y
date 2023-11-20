#include "rendering/render_commands.hpp"

#include "core/log.hpp"
#include "core/resource_handler.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/font.hpp"
#include "rendering/model.hpp"
#include "rendering/camera.hpp"
#include "scene/components.hpp"

namespace EngineY {
    
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
    
    void DrawTextComponent::BufferGlyphData(
        const msdf_atlas::GlyphGeometry* glyph , 
        const glm::vec2& offset , float fscale
    ) {
        double al , ab , ar , at;
        glyph->getQuadAtlasBounds(al , ab , ar , at);
        glm::vec2 tex_min((float)al , (float)ab);
        glm::vec2 tex_max((float)ar , (float)at);

        double pl , pb , pr , pt;
        glyph->getQuadPlaneBounds(pl , pb , pr , pt);
        glm::vec2 pos_min((float)pl , (float)pb);
        glm::vec2 pos_max((float)pr , (float)pt);

        Texture* atlas_texture = text_component.font_atlas->GetAtlasTexture();
        float texel_width = 1.f / atlas_texture->Size().x;
        float texel_height = 1.f / atlas_texture->Size().y;
        tex_min *= glm::vec2(texel_width , texel_height);
        tex_max *= glm::vec2(texel_width , texel_height);

        pos_min *= fscale , pos_max *= fscale;
        pos_min += offset , pos_max += offset;

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

        text_component.vao->SetData(vertices , indices , { 3 , 2 });
    }
    
    float DrawTextComponent::AdjustSpacing(
        const msdf_atlas::FontGeometry& font_geometry ,     
        float fscale , double space_advance , 
        size_t char_index
    ) {
        float advance = space_advance;
        if (char_index < text_component.text.size() - 1) {
            char next_char = text_component.text[char_index + 1];
            double real_advance;
            font_geometry.getAdvance(real_advance , text_component.text[char_index] , next_char);
            advance = (float)real_advance;
        }
        return fscale * advance + text_component.kerning_offset;
    }
    
    DrawTextComponent::DrawTextComponent(components::TextComponent& text , const glm::mat4& model)
            : text_component(text) , model(model) {
        if (text_component.vao == nullptr)
            text_component.vao = ynew(VertexArray , 5 * 4 * sizeof(float));
    }

    void DrawTextComponent::Execute(Camera* camera , const ShaderUniforms& uniforms) {
        if (text_component.font_atlas == nullptr) {
            ENGINE_WARN("Attempting to render text with a null font altas");
            text_component.corrupted = true;
            return;
        }

        if (text_component.shader == nullptr)
            text_component.shader = ResourceHandler::Instance()->GetShader(text_component.shader_name);
        if (text_component.shader == nullptr) {
            text_component.shader = ResourceHandler::Instance()->GetCoreShader(text_component.shader_name);

            if (text_component.shader == nullptr) {
                ENGINE_WARN("Failed to execute DrawTextComponent :: Shader [{0}] is null" , text_component.shader_name);
                text_component.corrupted = true;
                return;
            }
        }
        
        Texture* font_atlas = text_component.font_atlas->GetAtlasTexture();
        if (font_atlas == nullptr) {
            ENGINE_WARN("Failed to execute DrawTextComponent :: Font atlas is null");
            return;
        }

        const auto& font_geometry = text_component.font_atlas->GetGeometryData().font_geometry;
        const auto& metrics = font_geometry.getMetrics();

        const glm::ivec2& atlas_size = font_atlas->Size();

        double x = 0.f;
        double fscale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.f;
        
        const float space_advance = font_geometry.getGlyph(' ')->getAdvance();

        for (size_t i = 0; i < text_component.text.size(); ++i) {
            char c = text_component.text[i];

            if (c == '\r') continue;

            if (c == '\n') {
                x = 0;
                y -= fscale * metrics.lineHeight + text_component.line_spacing; 
                continue;
            }

            if (c == ' ') {
                x += AdjustSpacing(font_geometry , fscale , space_advance , i);
                continue;
            }

            if (c == '\t') {
                for (uint32_t j = 0; j < 4; ++j) {
                    x += AdjustSpacing(font_geometry , fscale , space_advance , i);
                    if (i < text_component.text.size() - 1)
                        ++i;
                    else 
                        break;
                }
                continue;
            }

            const msdf_atlas::GlyphGeometry* glyph = font_geometry.getGlyph(c);
            if (glyph == nullptr) {
                glyph = font_geometry.getGlyph('?');
            } 

            if (glyph == nullptr) {
                ENGINE_WARN("Rendering corrupted font atlas :: [{}]" , text_component.font_atlas->Path().string());
                return;
            }

            BufferGlyphData(glyph , { x , y } , fscale);

            if (text_component.vao->Valid()) {
                text_component.shader->Bind();
                text_component.shader->SetUniformVec4("bgcolor" , text_component.background_color);
                text_component.shader->SetUniformVec4("fgcolor" , text_component.text_color);
                text_component.shader->SetUniformInt("msdf" , 0);
                font_atlas->Bind(0);
                SetCameraUniforms(text_component.shader , camera);
                text_component.shader->SetUniformMat4("model" , model);
                text_component.vao->Draw(DrawMode::TRIANGLES);
                font_atlas->Unbind(0);
                text_component.shader->Unbind();
            }

            if (i < text_component.text.size() - 1) {
                double advance = glyph->getAdvance();
                char next = text_component.text[i + 1];
                font_geometry.getAdvance(advance , c , next);

                x += advance * fscale + text_component.kerning_offset;
            }
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

        ENGINE_ASSERT(renderable.shader != nullptr , "Shader is null | UNDEFINED BEHAVIOUR");

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
