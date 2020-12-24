#include "../include/yds_d3d11_shader_program.h"
#include "../include/yds_d3d11_shader.h"

ysD3D11ShaderProgram::ysD3D11ShaderProgram() : ysShaderProgram(DeviceAPI::DirectX11) {
    /* void */
}

ysD3D11ShaderProgram::~ysD3D11ShaderProgram() {
    /* void */
}

ysD3D11Shader *ysD3D11ShaderProgram::GetShader(ysShader::ShaderType type) {
    return static_cast<ysD3D11Shader *>(m_shaderSlots[(int)type]);
}
