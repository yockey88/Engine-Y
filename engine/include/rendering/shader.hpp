#ifndef YE_SHADER_HPP
#define YE_SHADER_HPP

#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "core/UUID.hpp"

namespace YE {

    struct Material {
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        float shininess = 32.0f;
    };
    
    enum class UniformType {
        INT   = 0 ,
        FLOAT ,
        VEC2  ,
        VEC3  ,
        VEC4  ,
        MAT3  ,
        MAT4  ,
    };

    // using UniformData = std::variant<
    //     int32_t , float , 
    //     glm::vec2 , glm::vec3 , glm::vec4 , 
    //     glm::mat3 , glm::mat4
    // >;

    struct UniformData {
        UniformType type;
        void* data_handle;
    };

    struct Uniform {
        std::string name;
        UniformData data;
    };

    struct ShaderUniforms {
        std::string shader_name;
        std::vector<Uniform> uniforms;
    };

    class Shader;

    struct ShaderResource {
        std::string name;
        std::string vert_filename;
        std::string frag_filename;
        std::string geom_filename;
        std::string vert_path;
        std::string frag_path;
        std::string geom_path;

        bool has_geom = false;

        Shader* shader = nullptr;

        ShaderResource() {}
    };

    class Shader {

        enum ShaderType {
            VERTEX = GL_VERTEX_SHADER ,
            FRAGMENT = GL_FRAGMENT_SHADER ,
            GEOMETRY = GL_GEOMETRY_SHADER
        };
        
        uint32_t program = 0;

        bool valid = false;
        bool has_geometry = false;

        uint32_t vertex_shader = 0;
        uint32_t fragment_shader = 0;
        uint32_t geometry_shader = 0;
        
        std::string name;
        std::string vertex_path;
        std::string fragment_path;
        std::string geometry_path;

        std::unordered_map<UUID32 , int32_t> uniform_locations;

        uint32_t GetUniformLocation(const std::string& name);

        void ShaderError(ShaderType type , uint32_t shader);
        void CompileShader(ShaderType type , uint32_t& shader , const char* buffer);
        void Link();

        Shader(Shader&&) = delete;
        Shader(const Shader&) = delete;
        Shader& operator=(Shader&&) = delete;
        Shader& operator=(const Shader&) = delete;

        public:
            Shader(const std::string& vertex_path , const std::string& fragment_path , const std::string& geometry_path = "") 
                : vertex_path(vertex_path) , fragment_path(fragment_path) , geometry_path(geometry_path) , 
                has_geometry(geometry_path == "" ? false : true) {}
            ~Shader();

            bool Compile();

            void SetUniform(const Uniform& uniform);
            void SetUniformInt(const std::string& name , uint32_t val);
            void SetUniformFloat(const std::string& name , float val);
            void SetUniformVec2(const std::string& name , const glm::vec2& val);
            void SetUniformVec3(const std::string& name , const glm::vec3& val);
            void SetUniformVec4(const std::string& name , const glm::vec4& val);
            void SetUniformMat3(const std::string& name , const glm::mat3& mat);
            void SetUniformMat4(const std::string& name , const glm::mat4& mat);

            inline void Bind() const { glUseProgram(program); }
            inline void Unbind() const { glUseProgram(0); }

            inline std::string Name() const { return name; }
            inline void SetName(const std::string& name) { this->name = name; }

    };

}

#endif // !YE_SHADER_HPP