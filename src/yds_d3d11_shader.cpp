#if defined(_WIN64)

#include "../include/yds_d3d11_shader.h"

#include "../include/yds_d3d11_utilities.h"

ysD3D11Shader::ysD3D11Shader() : ysShader(DeviceAPI::DirectX11) {
    m_geometryShader = nullptr;
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_shaderByteCode = nullptr;
    m_shaderSize = 0U;
}

ysD3D11Shader::~ysD3D11Shader() {
    /* void */
}

void ysD3D11Shader::SetDebugName(const std::string &debugName) {
    ysShader::SetDebugName(debugName);

    switch (GetShaderType()) {
    case ysShader::ShaderType::Vertex:
        D3D11SetDebugName(m_vertexShader, debugName);
        break;
    case ysShader::ShaderType::Pixel:
        D3D11SetDebugName(m_pixelShader, debugName);
        break;
    }
}

#endif /* Windows */
