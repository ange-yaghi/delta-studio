#include "../include/yds_d3d10_shader.h"

ysD3D10Shader::ysD3D10Shader() : ysShader(DeviceAPI::DirectX10) {
    m_shaderBlob = NULL;
    m_vertexShader = NULL;
}

ysD3D10Shader::~ysD3D10Shader() {
    /* void */
}
