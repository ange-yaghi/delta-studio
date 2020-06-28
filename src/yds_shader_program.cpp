#include "../include/yds_shader_program.h"

ysShaderProgram::ysShaderProgram() : ysContextObject("SHADER_PROGRAM", DeviceAPI::Unknown) {
    memset(m_shaderSlots, 0, sizeof(ysShader *) * (int)ysShader::ShaderType::NumShaderTypes);
    m_isLinked = false;
}

ysShaderProgram::ysShaderProgram(DeviceAPI API) : ysContextObject("SHADER_PROGRAM", API) {
    memset(m_shaderSlots, 0, sizeof(ysShader *) * (int)ysShader::ShaderType::NumShaderTypes);
    m_isLinked = false;
}

ysShaderProgram::~ysShaderProgram() {
    /* void */
}
