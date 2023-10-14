#include "rendering/gl_error_helper.hpp"

#include <glad/glad.h>

#include "log.hpp"

namespace YE {

    void CheckGLError(const char* file, int line) {
        GLenum err = glGetError();
        bool halt = (err != GL_NO_ERROR);
        while (err != GL_NO_ERROR) {
            switch (err) {
                case GL_INVALID_ENUM: 
                    YE_ERROR("OpenGL Error INVALID_ENUM :: [{0}:{1}]", file, line);
                break;
                case GL_INVALID_VALUE:
                    YE_ERROR("OpenGL Error INVALID_VALUE :: [{0}:{1}]", file, line);
                break;
                case GL_INVALID_OPERATION:
                    YE_ERROR("OpenGL Error INVALID_OPERATION :: [{0}:{1}]", file, line);
                break;
                case GL_STACK_OVERFLOW:
                    YE_ERROR("OpenGL Error STACK_OVERFLOW :: [{0}:{1}]", file, line);
                break;
                case GL_STACK_UNDERFLOW:
                    YE_ERROR("OpenGL Error STACK_UNDERFLOW :: [{0}:{1}]", file, line);
                break;
                case GL_OUT_OF_MEMORY:
                    YE_ERROR("OpenGL Error OUT_OF_MEMORY :: [{0}:{1}]", file, line);
                break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    YE_ERROR("OpenGL Error INVALID_FRAMEBUFFER_OPERATION :: [{0}:{1}]", file, line);
                break;
                case GL_CONTEXT_LOST:
                    YE_ERROR("OpenGL Error CONTEXT_LOST :: [{0}:{1}]", file, line);
                break;
                case GL_TABLE_TOO_LARGE:
                    YE_ERROR("OpenGL Error TABLE_TOO_LARGE :: [{0}:{1}]", file, line);
                break;
                default: break;
            }
            err = glGetError();
        }

        YE_CRITICAL_ASSERTION(!halt, "OpenGL Error | Fatal");
    }

}