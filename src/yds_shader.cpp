#include "../include/yds_shader.h"

ysShader::ysShader() : ysContextObject("SHADER", API_UNKNOWN) {
    m_filename[0] = '\0';
    m_shaderName[0] = '\0';
    m_shaderType = SHADER_TYPE::SHADER_TYPE_VERTEX;
}

ysShader::ysShader(DEVICE_API API) : ysContextObject("SHADER", API) {
    m_filename[0] = '\0';
    m_shaderName[0] = '\0';
    m_shaderType = SHADER_TYPE::SHADER_TYPE_VERTEX;
}

ysShader::~ysShader() {
    /* void */
}
