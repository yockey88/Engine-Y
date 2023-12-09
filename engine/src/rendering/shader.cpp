#include "rendering/shader.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "core/log.hpp"
#include "core/hash.hpp"
#include "core/filesystem.hpp"

namespace EngineY {

    /// \todo save locations of shaders to avoid glGetUniformLocations call
    ///     every time a uniform is set
    uint32_t Shader::GetUniformLocation(const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        
        if (uniform_locations.find(id) != uniform_locations.end())
            return uniform_locations[id];

        int32_t location = glGetUniformLocation(program , name.c_str());
        uniform_locations[id] = location;
        return location;
    }
    
    void Shader::ShaderError(ShaderType type , uint32_t shader) {
        char info_log[512];
        glGetShaderInfoLog(shader , 512 , nullptr , info_log);
        std::string path = "";
        switch (type) {
            case ShaderType::VERTEX: path = vertex_path; break;
            case ShaderType::FRAGMENT: path = fragment_path; break;
            case ShaderType::GEOMETRY: path = geometry_path; break;
            default: path = "UNKNOWN SHADER TYPE"; break;
        };
        ENGINE_WARN("Failed to compile shader :: [{0}]" , path);
        std::cout << "\t" << info_log << "\n";
        valid = false;
    }

    void Shader::CompileShader(ShaderType type , uint32_t& shader , const char* buffer) {
        if (ShaderType::GEOMETRY == type && (!has_geometry || geometry_path == "")) {
            ENGINE_WARN("Failed to compile geometry shader :: [{0}]" , geometry_path);
            valid = false;
        } else {
            int32_t compile_check = 0;

            shader = glCreateShader(type);
            glShaderSource(shader , 1 , &buffer , nullptr);
            glCompileShader(shader);
            glGetShaderiv(shader , GL_COMPILE_STATUS , &compile_check);
            if (compile_check != GL_TRUE)
                ShaderError(type , shader);
        }
    }
    
    void Shader::Link() {
        int32_t compile_check = 0;

        program = glCreateProgram();
        glAttachShader(program , vertex_shader);
        glAttachShader(program , fragment_shader);
        if (has_geometry)
            glAttachShader(program , geometry_shader);

        glLinkProgram(program);
        glValidateProgram(program);
        glGetProgramiv(program , GL_LINK_STATUS , &compile_check);
        if (compile_check != GL_TRUE) {
            char info_log[512];
            glGetProgramInfoLog(program , 512 , nullptr , info_log);
            ENGINE_WARN("Failed to link shader program");
            ENGINE_ERROR("Error: {0}" , info_log);
        } else {
            valid = true;
        }
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        if (has_geometry)
            glDeleteShader(geometry_shader);
    }
    
    void Shader::DeleteProgram() {
        glUseProgram(0);
        glDeleteProgram(program);

        valid = false;
    }

    Shader::~Shader() {
        DeleteProgram();
    }
    
    bool Shader::Compile() {
        vertex_src =  EngineY::Filesystem::ReadFileAsStr(vertex_path);
        fragment_src =  EngineY::Filesystem::ReadFileAsStr(fragment_path);
        geometry_src = has_geometry ?  EngineY::Filesystem::ReadFileAsStr(geometry_path) : "";

        const char* vert_src = vertex_src.c_str();
        const char* frag_src = fragment_src.c_str();
        const char* geom_src = geometry_src.c_str();

        CompileShader(ShaderType::VERTEX , vertex_shader , vert_src);
        CompileShader(ShaderType::FRAGMENT , fragment_shader , frag_src);
        if (has_geometry)
            CompileShader(ShaderType::GEOMETRY , geometry_shader , geom_src);

        Link();

        dirty = false;
        
        return valid;
    }
    
    void Shader::SetUniform(const Uniform& uniform) {
        ENGINE_ASSERT(uniform.data.data_handle != nullptr , "Attempting to set uniform with no data");

        uint32_t ival;
        float fval;
        glm::vec2 vec2;
        glm::vec3 vec3;
        glm::vec4 vec4;
        glm::mat3 mat3;
        glm::mat4 mat4;
        switch (uniform.data.type) {
            case UniformType::INT: 
                ival = *static_cast<uint32_t*>(uniform.data.data_handle);
                SetUniformInt(uniform.name , ival);
            break;
            case UniformType::FLOAT: 
                fval = *static_cast<float*>(uniform.data.data_handle);
                SetUniformFloat(uniform.name , fval);
            break;
            case UniformType::VEC2: 
                vec2 = *static_cast<glm::vec2*>(uniform.data.data_handle);
                SetUniformVec2(uniform.name , vec2);
            break;
            case UniformType::VEC3: 
                vec3 = *static_cast<glm::vec3*>(uniform.data.data_handle);
                SetUniformVec3(uniform.name , vec3);
            break;
            case UniformType::VEC4: 
                vec4 = *static_cast<glm::vec4*>(uniform.data.data_handle);
                SetUniformVec4(uniform.name , vec4);
            break;
            case UniformType::MAT3: 
                mat3 = *static_cast<glm::mat3*>(uniform.data.data_handle);
                SetUniformMat3(uniform.name , mat3);
            break;
            case UniformType::MAT4: 
                mat4 = *static_cast<glm::mat4*>(uniform.data.data_handle);
                SetUniformMat4(uniform.name , mat4);
            break;
            default:
                ENGINE_ASSERT(false , "Invalid uniform type");
        }
    }

    void Shader::SetUniformInt(const std::string& name , uint32_t val) {
        glUniform1i(GetUniformLocation(name) , val);
    }

    void Shader::SetUniformFloat(const std::string& name , float val) {
        glUniform1f(GetUniformLocation(name) , val);
    }

    void Shader::SetUniformVec2(const std::string& name , const glm::vec2& val) {
        glUniform2f(GetUniformLocation(name) , val.x , val.y);
    }

    void Shader::SetUniformVec3(const std::string& name , const glm::vec3& val) {
        glUniform3f(GetUniformLocation(name) , val.x , val.y , val.z);
    }

    void Shader::SetUniformVec4(const std::string& name , const glm::vec4& val) {
        glUniform4f(GetUniformLocation(name) , val.x , val.y , val.z , val.w);
    }

    void Shader::SetUniformMat3(const std::string& name , const glm::mat3& mat) {
        glUniformMatrix3fv(GetUniformLocation(name) , 1 , GL_FALSE , glm::value_ptr(mat));
    }
    
    void Shader::SetUniformMat4(const std::string& name , const glm::mat4& mat) {
        glUniformMatrix4fv(GetUniformLocation(name) , 1 , GL_FALSE , glm::value_ptr(mat));
    }
    
}
