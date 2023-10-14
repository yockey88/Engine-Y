#ifndef YE_RENDER_COMMANDS_HPP
#define YE_RENDER_COMMANDS_HPP

#include <memory>
#include <vector>
#include <utility>

#include <glm/glm.hpp>

#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/model.hpp"
#include "rendering/camera.hpp"

namespace YE {

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

    class DrawVao : public RenderCommand {
        VertexArray* vao = nullptr;
        Shader* shader = nullptr;
        glm::mat4 model;
        DrawMode mode;

        public:
            DrawVao(VertexArray* vao , Shader* shader , const glm::mat4& model , DrawMode mode = DrawMode::TRIANGLES) 
                    : vao(vao) , shader(shader) , model(model) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawRenderable : public RenderCommand {
        components::Renderable& renderable;
        const glm::mat4 model;
        DrawMode mode;

        public:
            DrawRenderable(components::Renderable& renderable , const glm::mat4& model , 
                    DrawMode mode = DrawMode::TRIANGLES) 
                    : renderable(renderable) , model(model) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;

    };

    class DrawTexturedRenderable : public RenderCommand {
        components::TexturedRenderable& renderable;
        const glm::mat4 model;
        DrawMode mode;

        public:
            DrawTexturedRenderable(components::TexturedRenderable& renderable , const glm::mat4& model ,
                            DrawMode mode = DrawMode::TRIANGLES) 
                            : renderable(renderable) , model(model) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawRenderableModel : public RenderCommand {
        components::RenderableModel& renderable;
        const glm::mat4 model_matrix;
        DrawMode mode;

        public:
            DrawRenderableModel(components::RenderableModel& renderable , const glm::mat4& model_matrix , 
                      DrawMode mode = DrawMode::TRIANGLES) 
                      : renderable(renderable) , model_matrix(model_matrix) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

    class DrawPointLight : public RenderCommand {
        components::Renderable& renderable;
        components::PointLight* light = nullptr;
        const glm::mat4 model_matrix;
        DrawMode mode;

        public:
            DrawPointLight(components::Renderable& renderable , components::PointLight* light , const glm::mat4& model_matrix , 
                           DrawMode mode = DrawMode::TRIANGLES) 
                           : renderable(renderable) , light(light) , model_matrix(model_matrix) , mode(mode) {}
            virtual void Execute(Camera* camera , const ShaderUniforms& uniforms) override;
    };

}

#endif // !YE_RENDER_COMMAND_HPP