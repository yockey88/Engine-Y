#ifndef ENGINEY_GL_ERROR_HELPER_HPP
#define ENGINEY_GL_ERROR_HELPER_HPP

namespace EngineY {

    void CheckGLError(const char* file, int line);

}

#define CHECK_GL  EngineY::CheckGLError(__FILE__, __LINE__);

#endif // !YE_GL_ERROR_HELPER_HPP