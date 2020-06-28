#include "../include/yds_opengl_shader_program.h"

#include "../include/yds_opengl_shader.h"

ysOpenGLShaderProgram::ysOpenGLShaderProgram() : ysShaderProgram(DeviceAPI::OpenGL4_0) {
    m_handle = 0;
}

ysOpenGLShaderProgram::~ysOpenGLShaderProgram() {
    /* void */
}

ysOpenGLShader *ysOpenGLShaderProgram::GetShader(ysShader::ShaderType type) {
    return static_cast<ysOpenGLShader *>(m_shaderSlots[(int)type]);
}
