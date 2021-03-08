#include "../include/default_shaders.h"

dbasic::DefaultShaders::DefaultShaders() {
    m_lightCount = 0;

    m_cameraPosition = ysMath::LoadVector(0.0f, 0.0f, 10.0f);
    m_cameraTarget = ysMath::Constants::Zero3;
    m_cameraMode = CameraMode::Flat;
    m_cameraAngle = 0.0f;

    m_cameraFov = ysMath::Constants::PI / 3.0f;

    m_cameraUp = ysMath::Constants::ZAxis;
    m_screenHeight = 1.0f;
    m_screenWidth = 1.0f;

    m_nearClip = 2.0f;
    m_farClip = 100.0f;

    m_mainStage = nullptr;
}

dbasic::DefaultShaders::~DefaultShaders() {
    /* void */
}

ysError dbasic::DefaultShaders::Initialize(ShaderSet *shaderSet, ysRenderTarget *renderTarget, ysShaderProgram *shaderProgram, ysInputLayout *inputLayout) {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(shaderSet->NewStage("ShaderStage::Main", &m_mainStage));

    m_mainStage->SetInputLayout(inputLayout);
    m_mainStage->SetRenderTarget(renderTarget);
    m_mainStage->SetShaderProgram(shaderProgram);
    m_mainStage->SetType(ShaderStage::Type::FullPass);

    m_mainStage->NewConstantBuffer<ShaderScreenVariables>(
        "Buffer::ScreenData", 0, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_shaderScreenVariables);
    m_mainStage->NewConstantBuffer<ShaderObjectVariables>(
        "Buffer::ObjectData", 1, ShaderStage::ConstantBufferBinding::BufferType::ObjectData, &m_shaderObjectVariables);
    m_mainStage->NewConstantBuffer<LightingControls>(
        "Buffer::LightingData", 3, ShaderStage::ConstantBufferBinding::BufferType::SceneData, &m_lightingControls);

    m_mainStage->AddTextureInput(0, &m_mainStageDiffuseTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DefaultShaders::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    /* void */

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DefaultShaders::UseMaterial(Material *material) {
    YDS_ERROR_DECLARE("UseMaterial");

    if (material == nullptr) {
        SetBaseColor(ysMath::LoadVector(1.0f, 0.0f, 1.0f, 1.0f));
        SetColorReplace(true);
    }
    else {
        SetBaseColor(material->GetDiffuseColor());

        if (material->GetDiffuseMap() != nullptr) {
            SetColorReplace(false);
            SetDiffuseTexture(material->GetDiffuseMap());
        }
        else {
            SetColorReplace(true);
        }

        SetLit(material->IsLit());
    }

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

void dbasic::DefaultShaders::SetClearColor(const ysVector &color) {
    m_mainStage->SetClearColor(color);
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

void dbasic::DefaultShaders::SetCameraPosition(float x, float y) {
    const ysVector3 p = ysMath::GetVector3(m_cameraPosition);
    m_cameraPosition = ysMath::LoadVector(x, y, p.z, 1.0f);
}

void dbasic::DefaultShaders::SetCameraPosition(const ysVector &pos) {
    m_cameraPosition = pos;
}

ysVector dbasic::DefaultShaders::GetCameraPosition() const {
    return m_cameraPosition;
}

void dbasic::DefaultShaders::GetCameraPosition(float *x, float *y) const {
    *x = ysMath::GetX(m_cameraPosition);
    *y = ysMath::GetY(m_cameraPosition);
}

void dbasic::DefaultShaders::SetCameraUp(const ysVector &up) {
    m_cameraUp = up;
}

ysVector dbasic::DefaultShaders::GetCameraUp() const {
    return m_cameraUp;
}

void dbasic::DefaultShaders::SetCameraTarget(const ysVector &target) {
    m_cameraTarget = target;
}

void dbasic::DefaultShaders::SetCameraMode(CameraMode mode) {
    m_cameraMode = mode;
}

void dbasic::DefaultShaders::SetCameraAngle(float angle) {
    m_cameraAngle = angle;
}

float dbasic::DefaultShaders::GetCameraFov() const {
    return m_cameraFov;
}

void dbasic::DefaultShaders::SetCameraFov(float fov) {
    m_cameraFov = fov;
}

float dbasic::DefaultShaders::GetCameraAspect() const {
    return m_screenWidth / m_screenHeight;
}

void dbasic::DefaultShaders::SetCameraAltitude(float altitude) {
    const ysVector3 p = ysMath::GetVector3(m_cameraPosition);
    m_cameraPosition = ysMath::LoadVector(p.x, p.y, altitude, 1.0f);
}

float dbasic::DefaultShaders::GetCameraAltitude() const {
    return ysMath::GetZ(m_cameraPosition);
}

void dbasic::DefaultShaders::SetScreenDimensions(float width, float height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

void dbasic::DefaultShaders::CalculateCamera() {
    const float aspect = GetCameraAspect();
    const float sinRot = sin(m_cameraAngle * ysMath::Constants::PI / 180.0f);
    const float cosRot = cos(m_cameraAngle * ysMath::Constants::PI / 180.0f);

    SetProjection(ysMath::Transpose(ysMath::FrustrumPerspective(m_cameraFov, aspect, m_nearClip, m_farClip)));

    const ysVector cameraEye = m_cameraPosition;
    ysVector cameraTarget;
    ysVector up;

    if (m_cameraMode == CameraMode::Flat) {
        cameraTarget = ysMath::Mask(cameraEye, ysMath::Constants::MaskOffZ);
        up = ysMath::LoadVector(-sinRot, cosRot);
    }
    else {
        cameraTarget = m_cameraTarget;
        const ysVector cameraDir = ysMath::Sub(cameraTarget, cameraEye);
        const ysVector right = ysMath::Cross(cameraDir, m_cameraUp);
        up = ysMath::Normalize(ysMath::Cross(right, cameraDir));
    }

    SetCameraView(ysMath::Transpose(ysMath::CameraTarget(cameraEye, cameraTarget, up)));
    SetEye(cameraEye);
}

void dbasic::DefaultShaders::CalculateUiCamera() {
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

void dbasic::DefaultShaders::ConfigureFlags(int regularFlagIndex, int riggedFlagIndex) {
    m_mainStage->SetFlagBit(regularFlagIndex);
}

dbasic::StageEnableFlags dbasic::DefaultShaders::GetRegularFlags() const {
    return m_mainStage->GetFlags();
}

dbasic::StageEnableFlags dbasic::DefaultShaders::GetRiggedFlags() const {
    return 0;
}

void dbasic::DefaultShaders::ConfigureImage(
    float scaleX, float scaleY,
    float texOffsetU, float texOffsetV,
    float texScaleU, float texScaleV)
{
    SetScale(scaleX, scaleY);
    SetTexOffset(texOffsetU, texOffsetV);
    SetTexScale(texScaleU, texScaleV);
    SetColorReplace(false);
    SetLit(true);
}

void dbasic::DefaultShaders::ConfigureBox(float width, float height) {
    SetScale(width / 2.0f, height / 2.0f);
    SetTexOffset(0.0f, 0.0f);
    SetTexScale(1.0f, 1.0f);
    SetColorReplace(true);
}

void dbasic::DefaultShaders::ConfigureAxis(
    const ysVector &position, const ysVector &direction, float length)
{
    ysMatrix trans = ysMath::TranslationTransform(position);
    ysMatrix offset = ysMath::TranslationTransform(ysMath::LoadVector(0, length / 2.0f));

    ysVector orth = ysMath::Cross(direction, ysMath::Constants::ZAxis);
    ysMatrix dir = ysMath::LoadMatrix(orth, direction, ysMath::Constants::ZAxis, ysMath::Constants::IdentityRow4);
    dir = ysMath::Transpose(dir);

    ysMatrix transform = ysMath::MatMult(trans, dir);
    transform = ysMath::MatMult(transform, offset);
    SetObjectTransform(transform);
}

void dbasic::DefaultShaders::ConfigureModel(float scale, ModelAsset *model) {
    SetScale(scale, scale, scale);
    SetTexOffset(0.0f, 0.0f);
    SetTexScale(1.0f, 1.0f);
    UseMaterial(model->GetMaterial());
}

void dbasic::DefaultShaders::SetDiffuseTexture(ysTexture *texture) {
    m_mainStage->BindTexture(texture, m_mainStageDiffuseTexture);
}
