#include "../include/yds_shader_program.h"

ysShaderProgram::ysShaderProgram() : ysContextObject("SHADER_PROGRAM", API_UNKNOWN) {
    memset(m_shaderSlots, 0, sizeof(ysShader *) * (int)ysShader::ShaderType::NumShaderTypes);
    m_isLinked = false;
}

ysShaderProgram::ysShaderProgram(DEVICE_API API) : ysContextObject("SHADER_PROGRAM", API) {
    memset(m_shaderSlots, 0, sizeof(ysShader *) * (int)ysShader::ShaderType::NumShaderTypes);
    m_isLinked = false;
}

ysShaderProgram::~ysShaderProgram() {
    /* void */
}
