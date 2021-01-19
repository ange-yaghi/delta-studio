#include "../include/default_ui_shaders.h"

dbasic::DefaultUiShaders::DefaultUiShaders() {
    m_cameraAngle = 0.0f;

    m_screenWidth = 1.0f;
    m_screenHeight = 1.0f;
}

dbasic::DefaultUiShaders::~DefaultUiShaders() {
    /* void */
}

ysError dbasic::DefaultUiShaders::Initialize(ysDevice *device, ysRenderTarget *renderTarget, ysShaderProgram *shaderProgram, ysInputLayout *inputLayout) {
    YDS_ERROR_DECLARE("Initialize");

    m_shaderSet.Initialize(device);

    ShaderStage *mainStage = nullptr;
    YDS_NESTED_ERROR_CALL(m_shaderSet.NewStage("DefaultUiShaders::Main", &mainStage));

    mainStage->SetInputLayout(inputLayout);
    mainStage->SetRenderTarget(renderTarget);
    mainStage->SetShaderProgram(shaderProgram);
    mainStage->SetType(ShaderStage::Type::FullPass);

    mainStage->NewConstantBuffer<ShaderScreenVariables>(
        "DefaultUiShaders::ScreenData", 0, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_shaderScreenVariables);
    mainStage->NewConstantBuffer<ShaderObjectVariables>(
        "DefaultUiShaders::ObjectData", 1, ShaderStage::ConstantBufferBinding::BufferType::ObjectData, &m_shaderObjectVariables);
    mainStage->NewConstantBuffer<LightingControls>(
        "DefaultUiShaders::LightingData", 3, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_lightingControls);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DefaultUiShaders::Destroy() {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(m_shaderSet.Destroy());

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::DefaultUiShaders::SetScreenDimensions(float width, float height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

void dbasic::DefaultUiShaders::CalculateCamera() {
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
