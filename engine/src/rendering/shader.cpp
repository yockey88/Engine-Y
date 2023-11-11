#include "rendering/shader.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "log.hpp"
#include "core/hash.hpp"
#include "core/filesystem.hpp"
#include "rendering/gl_error_helper.hpp"

namespace YE {

    uint32_t Shader::GetUniformLocation(const std::string& name) {
        ENTER_FUNCTION_TRACE();

        UUID32 id = Hash::FNV32(name);
        
        if (uniform_locations.find(id) != uniform_locations.end())
            return uniform_locations[id];

        int32_t location = glGetUniformLocation(program , name.c_str());
        uniform_locations[id] = location;

        EXIT_FUNCTION_TRACE();
        return location;
    }
    
    void Shader::ShaderError(ShaderType type , uint32_t shader) {
        ENTER_FUNCTION_TRACE();

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

        EXIT_FUNCTION_TRACE();
    }

    void Shader::CompileShader(ShaderType type , uint32_t& shader , const char* buffer) {
        ENTER_FUNCTION_TRACE();

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

        EXIT_FUNCTION_TRACE();
    }
    
    void Shader::Link() {
        ENTER_FUNCTION_TRACE();

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
            valid = false;
        }
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        if (has_geometry)
            glDeleteShader(geometry_shader);

        EXIT_FUNCTION_TRACE();
    }

    Shader::~Shader() {
        glUseProgram(0);
        glDeleteProgram(program);
    }
    
    bool Shader::Compile() {
        ENTER_FUNCTION_TRACE();

        valid = true;

        std::string vert_str = YE::Filesystem::ReadFileAsStr(vertex_path);
        std::string frag_str = YE::Filesystem::ReadFileAsStr(fragment_path);
        std::string geom_str = has_geometry ? YE::Filesystem::ReadFileAsStr(geometry_path) : "";

        const char* vert_src = vert_str.c_str();
        const char* frag_src = frag_str.c_str();
        const char* geom_src = geom_str.c_str();

        int32_t compile_check = 0;

        CompileShader(ShaderType::VERTEX , vertex_shader , vert_src);
        CompileShader(ShaderType::FRAGMENT , fragment_shader , frag_src);
        if (has_geometry)
            CompileShader(ShaderType::GEOMETRY , geometry_shader , geom_src);

        Link();

        EXIT_FUNCTION_TRACE();
        return valid;
    }
    
    void Shader::SetUniform(const Uniform& uniform) {
        ENTER_FUNCTION_TRACE();

        YE_CRITICAL_ASSERTION(uniform.data.data_handle != nullptr , "Attempting to set uniform with no data");

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
                YE_CRITICAL_ASSERTION(false , "Invalid uniform type");
        }

        EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformInt(const std::string& name , uint32_t val) {
		ENTER_FUNCTION_TRACE();

        glUniform1i(GetUniformLocation(name) , val);

		EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformFloat(const std::string& name , float val) {
		ENTER_FUNCTION_TRACE();

        glUniform1f(GetUniformLocation(name) , val);

		EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformVec2(const std::string& name , const glm::vec2& val) {
		ENTER_FUNCTION_TRACE();

        glUniform2f(GetUniformLocation(name) , val.x , val.y);

		EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformVec3(const std::string& name , const glm::vec3& val) {
		ENTER_FUNCTION_TRACE();
  
        glUniform3f(GetUniformLocation(name) , val.x , val.y , val.z);
		
        EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformVec4(const std::string& name , const glm::vec4& val) {
		ENTER_FUNCTION_TRACE();
        
        glUniform4f(GetUniformLocation(name) , val.x , val.y , val.z , val.w);
        
		EXIT_FUNCTION_TRACE();
    }

    void Shader::SetUniformMat3(const std::string& name , const glm::mat3& mat) {
		ENTER_FUNCTION_TRACE();
  
        glUniformMatrix3fv(GetUniformLocation(name) , 1 , GL_FALSE , glm::value_ptr(mat));
		
        EXIT_FUNCTION_TRACE();
    }
    
    void Shader::SetUniformMat4(const std::string& name , const glm::mat4& mat) {
		ENTER_FUNCTION_TRACE();
  
        glUniformMatrix4fv(GetUniformLocation(name) , 1 , GL_FALSE , glm::value_ptr(mat));

		EXIT_FUNCTION_TRACE();
    }
    
}