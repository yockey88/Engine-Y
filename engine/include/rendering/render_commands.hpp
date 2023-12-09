#ifndef ENGINEY_RENDER_COMMANDS_HPP
#define ENGINEY_RENDER_COMMANDS_HPP

#include <glm/glm.hpp>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "core/reference.hpp"
#include "rendering/vertex_array.hpp"

namespace EngineY {

    struct ShaderUniforms;

    class VertexArray;
    class Shader;
    class Texture;
    class Model;
    class Camera;
    class Font;

namespace components {
    
    struct Renderable;
    struct TexturedRenderable;
    struct RenderableModel;
    struct TextComponent;
    struct PointLight;

}

    class RenderCommand {
        protected:
            void SetCameraUniforms(Ref<Shader> shader , Camera* camera);
        public:
            virtual ~RenderCommand() {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) = 0;
    };

    /* Eventually ::
        using ColoredGlyph = std::pair<color , glyph>;
        RenderText {
            std::vector<ColoredGlyph> glyphs;
        }
        FontRenderable {
            Font* font;
            RenderText text; 
            Shader* shader;
            std::string shader_name;
        };
    */

    class DrawTextComponent : public RenderCommand {
        VertexArray* vao = nullptr;
        components::TextComponent& text_component;
        const glm::mat4& model; 

        // this function is called for every glyph which seems slow, need to set up
        //      a buffer that buffers all vertex data on creation that we simply render 
        //      during Execute
        void BufferGlyphData(
            const msdf_atlas::GlyphGeometry* glyph , 
            const glm::vec2& offset , float fscale
        );

        float AdjustSpacing(
            const msdf_atlas::FontGeometry& font_geometry ,     
            float fscale , double space_advance , 
            size_t char_index
        );

        public:
            DrawTextComponent(components::TextComponent& text , const glm::mat4& model);
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawRenderable : public RenderCommand {
        components::Renderable& renderable;
        const glm::mat4& model;
        DrawMode mode;

        public:
            DrawRenderable(
                components::Renderable& renderable , const glm::mat4& model , 
                DrawMode mode = DrawMode::TRIANGLES
            ) : renderable(renderable) , model(model) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;

    };

    class DrawTexturedRenderable : public RenderCommand {
        components::TexturedRenderable& renderable;
        const glm::mat4& model;
        DrawMode mode;

        public:
            DrawTexturedRenderable(
                components::TexturedRenderable& renderable , const glm::mat4& model ,
                DrawMode mode = DrawMode::TRIANGLES
            ) : renderable(renderable) , model(model) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawRenderableModel : public RenderCommand {
        components::RenderableModel& renderable;
        const glm::mat4& model_matrix;
        DrawMode mode;

        public:
            DrawRenderableModel(
                components::RenderableModel& renderable , const glm::mat4& model_matrix , 
                DrawMode mode = DrawMode::TRIANGLES
            ) : renderable(renderable) , model_matrix(model_matrix) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawPointLight : public RenderCommand {
        components::Renderable& renderable;
        components::PointLight* light = nullptr;
        const glm::mat4& model_matrix;
        DrawMode mode;

        public:
            DrawPointLight(
                components::Renderable& renderable , components::PointLight* light , const glm::mat4& model_matrix , 
                DrawMode mode = DrawMode::TRIANGLES
            ) : renderable(renderable) , light(light) , model_matrix(model_matrix) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

}

#endif // !ENGINEY_RENDER_COMMAND_HPP
