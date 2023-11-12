#ifndef YE_RENDER_COMMANDS_HPP
#define YE_RENDER_COMMANDS_HPP

#include <memory>
#include <vector>
#include <utility>

#include <glm/glm.hpp>

#include "core/resource_handler.hpp"

namespace YE {

    class Font;
    class VertexArray;
    class Shader;
    class Texture;
    class Model;
    class Camera;

namespace components {
    
    struct Renderable;
    struct TexturedRenderable;
    struct RenderableModel;
    struct PointLight;

}

    class RenderCommand {
        protected:
            void SetCameraUniforms(Shader* shader , Camera* camera);
        public:
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

    class DrawFont : public RenderCommand {
        VertexArray* vao = nullptr;
        Font* font = nullptr;
        Shader* shader = nullptr;
        std::string text;
        const glm::mat4& model; 
        DrawMode mode;

        public:
            DrawFont(
                Font* font , Shader* shader , const std::string& text , 
                const glm::mat4& model , DrawMode mode = DrawMode::TRIANGLES
            ) : vao(ResourceHandler::Instance()->GetPrimitiveVAO("quad")) , 
                font(font) , shader(shader) , text(text) , model(model) , mode(mode) {}
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

#endif // !YE_RENDER_COMMAND_HPP