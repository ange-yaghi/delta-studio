#include "../include/yds_shader.h"

ysShader::ysShader() : ysContextObject("SHADER", DeviceAPI::Unknown) {
    m_filename[0] = '\0';
    m_shaderName[0] = '\0';
    m_shaderType = ShaderType::Vertex;
}

ysShader::ysShader(DeviceAPI API) : ysContextObject("SHADER", API) {
    m_filename[0] = '\0';
    m_shaderName[0] = '\0';
    m_shaderType = ShaderType::Vertex;
}

ysShader::~ysShader() {
    /* void */
}
