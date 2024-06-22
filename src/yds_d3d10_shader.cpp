#if defined(_WIN64)

#include "../include/yds_d3d10_shader.h"

ysD3D10Shader::ysD3D10Shader() : ysShader(DeviceAPI::DirectX10) {
    m_shaderByteCode = nullptr;
    m_shaderSize = 0;
    m_vertexShader = nullptr;
}

ysD3D10Shader::~ysD3D10Shader() {
    /* void */
}

#endif /* Windows */
