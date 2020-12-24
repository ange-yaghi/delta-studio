#include "../include/yds_d3d10_shader_program.h"
#include "../include/yds_d3d10_shader.h"

ysD3D10ShaderProgram::ysD3D10ShaderProgram() : ysShaderProgram(DeviceAPI::DirectX10) {
    /* void */
}

ysD3D10ShaderProgram::~ysD3D10ShaderProgram() {
    /* void */
}

ysD3D10Shader *ysD3D10ShaderProgram::GetShader(ysShader::ShaderType type) {
    return static_cast<ysD3D10Shader *>(m_shaderSlots[(int)type]);
}
