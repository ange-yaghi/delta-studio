#include "../include/yds_opengl_shader.h"

ysOpenGLShader::ysOpenGLShader() : ysShader(DeviceAPI::OpenGL4_0) {
    m_handle = 0;
}

ysOpenGLShader::~ysOpenGLShader() {
    /* void */
}
