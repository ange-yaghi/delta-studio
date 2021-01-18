#include "../include/default_shaders.h"

dbasic::DefaultShaders::DefaultShaders() {
    m_lightCount = 0;
}

dbasic::DefaultShaders::~DefaultShaders() {
    /* void */
}

ysError dbasic::DefaultShaders::Initialize(ysDevice *device, ysRenderTarget *renderTarget, ysShaderProgram *shaderProgram, ysInputLayout *inputLayout) {
    YDS_ERROR_DECLARE("Initialize");

    m_shaderSet.Initialize(device);

    ShaderStage *mainStage = nullptr;
    YDS_NESTED_ERROR_CALL(m_shaderSet.NewStage("ShaderStage::Main", &mainStage));

    mainStage->SetInputLayout(inputLayout);
    mainStage->SetRenderTarget(renderTarget);
    mainStage->SetShaderProgram(shaderProgram);
    mainStage->SetType(ShaderStage::Type::FullPass);

    mainStage->NewConstantBuffer<ShaderScreenVariables>(
        "Buffer::ScreenData", 0, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_shaderScreenVariables);
    mainStage->NewConstantBuffer<ShaderObjectVariables>(
        "Buffer::ObjectData", 1, ShaderStage::ConstantBufferBinding::BufferType::ObjectData, &m_shaderObjectVariables);
    mainStage->NewConstantBuffer<LightingControls>(
        "Buffer::LightingData", 3, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_lightingControls);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DefaultShaders::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    m_shaderSet.Destroy();

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::DefaultShaders::ResetBrdfParameters() {
    m_shaderObjectVariables = ShaderObjectVariables();
}

void dbasic::DefaultShaders::SetBaseColor(const ysVector &color) {
    m_shaderObjectVariables.BaseColor = ysMath::GetVector4(color);
}

void dbasic::DefaultShaders::ResetBaseColor() {
    m_shaderObjectVariables.BaseColor = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void dbasic::DefaultShaders::SetScale(float x, float y, float z) {
    m_shaderObjectVariables.Scale[0] = x;
    m_shaderObjectVariables.Scale[1] = y;
    m_shaderObjectVariables.Scale[2] = z;
}

void dbasic::DefaultShaders::SetTexOffset(float u, float v) {
    m_shaderObjectVariables.TexOffset[0] = u;
    m_shaderObjectVariables.TexOffset[1] = v;
}

void dbasic::DefaultShaders::SetTexScale(float u, float v) {
    m_shaderObjectVariables.TexScale[0] = u;
    m_shaderObjectVariables.TexScale[1] = v;
}

void dbasic::DefaultShaders::SetColorReplace(bool colorReplace) {
    m_shaderObjectVariables.ColorReplace = (colorReplace) ? 1 : 0;
}

void dbasic::DefaultShaders::SetLit(bool lit) {
    m_shaderObjectVariables.Lit = (lit) ? 1 : 0;
}

void dbasic::DefaultShaders::SetEmission(const ysVector &emission) {
    m_shaderObjectVariables.Emission = ysMath::GetVector4(emission);
}

void dbasic::DefaultShaders::SetSpecularMix(float specularMix) {
    m_shaderObjectVariables.SpecularMix = specularMix;
}

void dbasic::DefaultShaders::SetDiffuseMix(float diffuseMix) {
    m_shaderObjectVariables.DiffuseMix = diffuseMix;
}

void dbasic::DefaultShaders::SetMetallic(float metallic) {
    m_shaderObjectVariables.Metallic = metallic;
}

void dbasic::DefaultShaders::SetDiffuseRoughness(float diffuseRoughness) {
    m_shaderObjectVariables.DiffuseRoughness = diffuseRoughness;
}

void dbasic::DefaultShaders::SetSpecularRoughness(float specularRoughness) {
    m_shaderObjectVariables.SpecularPower = ::pow(2.0f, 12.0f * (1.0f - specularRoughness));
}

void dbasic::DefaultShaders::SetSpecularPower(float power) {
    m_shaderObjectVariables.SpecularPower = power;
}

void dbasic::DefaultShaders::SetIncidentSpecular(float incidentSpecular) {
    m_shaderObjectVariables.IncidentSpecular = incidentSpecular;
}

void dbasic::DefaultShaders::SetFogNear(float fogNear) {
    m_shaderScreenVariables.FogNear = fogNear;
}

void dbasic::DefaultShaders::SetFogFar(float fogFar) {
    m_shaderScreenVariables.FogFar = fogFar;
}

void dbasic::DefaultShaders::SetFogColor(const ysVector &color) {
    m_shaderScreenVariables.FogColor = ysMath::GetVector4(color);
}

void dbasic::DefaultShaders::SetObjectTransform(const ysMatrix &mat) {
    m_shaderObjectVariables.Transform = mat;
}

void dbasic::DefaultShaders::SetPositionOffset(const ysVector &position) {
    m_shaderObjectVariables.Transform =
        ysMath::MatMult(m_shaderObjectVariables.Transform, ysMath::TranslationTransform(position));
}

void dbasic::DefaultShaders::SetProjection(const ysMatrix &mat) {
    m_shaderScreenVariables.Projection = mat;
}

void dbasic::DefaultShaders::SetCameraView(const ysMatrix &mat) {
    m_shaderScreenVariables.CameraView = mat;
}

void dbasic::DefaultShaders::SetEye(const ysVector &vec) {
    m_shaderScreenVariables.Eye = ysMath::GetVector4(vec);
}

ysError dbasic::DefaultShaders::AddLight(const Light &light) {
    YDS_ERROR_DECLARE("AddLight");

    if (m_lightCount >= LightingControls::MaxLights) return YDS_ERROR_RETURN(ysError::None);
    m_lightingControls.Lights[m_lightCount] = light;
    m_lightingControls.Lights[m_lightCount].Active = 1;
    ++m_lightCount;

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::DefaultShaders::ResetLights() {
    m_lightCount = 0;
    for (int i = 0; i < LightingControls::MaxLights; ++i) {
        m_lightingControls.Lights[i].Active = 0;
    }

    m_lightingControls.AmbientLighting = ysVector4(0.0f, 0.0f, 0.0f, 0.0f);
}

ysError dbasic::DefaultShaders::SetAmbientLight(const ysVector4 &ambient) {
    YDS_ERROR_DECLARE("SetAmbientLight");

    m_lightingControls.AmbientLighting = ambient;

    return YDS_ERROR_RETURN(ysError::None);
}
