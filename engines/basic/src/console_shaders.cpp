#include "../include/console_shaders.h"

dbasic::ConsoleShaders::ConsoleShaders() {
    m_cameraAngle = 0.0f;
    m_screenWidth = 1.0f;
    m_screenHeight = 1.0f;

    m_textureHandle = 0;

    m_mainStage = nullptr;
}

dbasic::ConsoleShaders::~ConsoleShaders() {
    /* void */
}

ysError dbasic::ConsoleShaders::Initialize(
    ShaderSet *shaderSet, ysRenderTarget *renderTarget, ysShaderProgram *shaderProgram, ysInputLayout *inputLayout) 
{
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(shaderSet->NewStage("ConsoleShaders::Main", &m_mainStage));

    m_mainStage->SetInputLayout(inputLayout);
    m_mainStage->SetRenderTarget(renderTarget);
    m_mainStage->SetShaderProgram(shaderProgram);
    m_mainStage->SetType(ShaderStage::Type::FullPass);
    m_mainStage->SetFlagBit(3);
    m_mainStage->SetClearTarget(false);

    m_mainStage->NewConstantBuffer<ShaderScreenVariables>(
        "DefaultUiShaders::ScreenData", 0, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_shaderScreenVariables);
    m_mainStage->NewConstantBuffer<ConsoleShaderObjectVariables>(
        "DefaultUiShaders::ObjectData", 1, ShaderStage::ConstantBufferBinding::BufferType::ObjectData, &m_shaderObjectVariables);

    m_mainStage->AddTextureInput(0, &m_textureHandle);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ConsoleShaders::Destroy() {
    YDS_ERROR_DECLARE("Initialize");

    /* void */

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::ConsoleShaders::SetScreenDimensions(float width, float height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

void dbasic::ConsoleShaders::CalculateCamera() {
    m_shaderScreenVariables.Projection = ysMath::Transpose(
        ysMath::OrthographicProjection(
            m_screenWidth,
            m_screenHeight,
            0.001f,
            500.0f));

    const float sinRot = sin(m_cameraAngle * ysMath::Constants::PI / 180.0f);
    const float cosRot = cos(m_cameraAngle * ysMath::Constants::PI / 180.0f);

    const ysVector cameraEye = ysMath::LoadVector(0.0f, 0.0f, 10.0f, 1.0f);
    const ysVector cameraTarget = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 1.0f);
    const ysVector up = ysMath::LoadVector(-sinRot, cosRot);

    m_shaderScreenVariables.CameraView = ysMath::Transpose(ysMath::CameraTarget(cameraEye, cameraTarget, up));
    m_shaderScreenVariables.Eye = ysMath::LoadVector(cameraEye);
}

void dbasic::ConsoleShaders::SetFlagBit(int bit) {
    m_mainStage->SetFlagBit(bit);
}

int dbasic::ConsoleShaders::GetFlagBit() const {
    return m_mainStage->GetFlagBit();
}

dbasic::StageEnableFlags dbasic::ConsoleShaders::GetFlags() const {
    return m_mainStage->GetFlags();
}

void dbasic::ConsoleShaders::SetTexture(ysTexture *texture) {
    m_mainStage->BindTexture(texture, m_textureHandle);
}
