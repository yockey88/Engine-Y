#ifndef YE_GL_ERROR_HELPER_HPP
#define YE_GL_ERROR_HELPER_HPP

namespace YE {

    void CheckGLError(const char* file, int line);

}

#define CHECK_GL YE::CheckGLError(__FILE__, __LINE__);

#endif // !YE_GL_ERROR_HELPER_HPP