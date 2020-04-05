#include "../include/delta_engine.h"

#include "../include/skeleton.h"
#include "../include/render_skeleton.h"
#include "../include/material.h"

#include <assert.h>

dbasic::DeltaEngine::DeltaEngine() {
    m_device = nullptr;

    m_windowSystem = nullptr;
    m_gameWindow = nullptr;

    m_renderingContext = nullptr;
    m_mainRenderTarget = nullptr;

    m_audioSystem = nullptr;

    m_mainVertexBuffer = nullptr;
    m_mainIndexBuffer = nullptr;

    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_shaderProgram = nullptr;

    m_inputLayout = nullptr;

    m_initialized = false;

    m_clearColor[0] = 0.0F;
    m_clearColor[1] = 0.0F;
    m_clearColor[2] = 0.0F;
    m_clearColor[3] = 1.0F;

    // Input system
    m_mainKeyboard = nullptr;
    m_inputSystem = nullptr;
    m_mainMouse = nullptr;

    // Shader Controls
    m_shaderObjectVariables.MulCol.Set(1.0f, 1.0f, 1.0f, 1.0f);
    m_shaderObjectVariables.ColorReplace = 0;
    m_shaderObjectVariables.Scale[0] = 1.0f;
    m_shaderObjectVariables.Scale[1] = 1.0f;
    m_shaderObjectVariables.Scale[2] = 1.0f;
    m_shaderObjectVariablesSync = false;

    m_shaderScreenVariablesSync = false;

    // Camera
    m_cameraX = 0.0f;
    m_cameraY = 0.0f;
    m_cameraAltitude = 10.0f;
    m_cameraAngle = 0.0f;

    m_currentTarget = DrawTarget::Main;
}

dbasic::DeltaEngine::~DeltaEngine() {
    assert(m_windowSystem == nullptr);
    assert(m_gameWindow == nullptr);
    assert(m_audioSystem == nullptr);
    assert(m_mainVertexBuffer == nullptr);
    assert(m_mainIndexBuffer == nullptr);
    assert(m_vertexShader == nullptr);
    assert(m_vertexSkinnedShader == nullptr);
    assert(m_pixelShader == nullptr);
    assert(m_shaderProgram == nullptr);
    assert(m_inputLayout == nullptr);
    assert(m_skinnedInputLayout == nullptr);
    assert(m_mainKeyboard == nullptr);
    assert(m_inputSystem == nullptr);
    assert(m_mainMouse == nullptr);
    assert(m_shaderObjectVariablesBuffer == nullptr);
    assert(m_shaderScreenVariablesBuffer == nullptr);
    assert(m_shaderSkinningControlsBuffer == nullptr);
}

ysError dbasic::DeltaEngine::CreateGameWindow(const char *title, void *instance, ysContextObject::DEVICE_API API, const char *shaderDirectory, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateGameWindow");

    // Create the window system
    YDS_NESTED_ERROR_CALL(ysWindowSystem::CreateWindowSystem(&m_windowSystem, ysWindowSystemObject::Platform::WINDOWS));
    m_windowSystem->ConnectInstance(instance);

    // Find the monitor setup
    m_windowSystem->SurveyMonitors();
    ysMonitor *mainMonitor = m_windowSystem->GetMonitor(0);

    YDS_NESTED_ERROR_CALL(ysInputSystem::CreateInputSystem(&m_inputSystem, ysWindowSystemObject::Platform::WINDOWS));
    m_windowSystem->AssignInputSystem(m_inputSystem);
    m_inputSystem->CreateDevices(false);

    m_mainKeyboard = m_inputSystem->GetMainDevice(ysInputDevice::InputDeviceType::KEYBOARD);
    m_mainMouse = m_inputSystem->GetMainDevice(ysInputDevice::InputDeviceType::MOUSE);

    m_mainKeyboard->AttachDependency();
    m_mainMouse->AttachDependency();

    // Create the game window
    YDS_NESTED_ERROR_CALL(m_windowSystem->NewWindow(&m_gameWindow));
    YDS_NESTED_ERROR_CALL(m_gameWindow->InitializeWindow(nullptr, title, ysWindow::WindowStyle::WINDOWED, mainMonitor));
    m_gameWindow->AttachEventHandler(&m_windowHandler);

    // Create the graphics device
    YDS_NESTED_ERROR_CALL(ysDevice::CreateDevice(&m_device, API));
    YDS_NESTED_ERROR_CALL(m_device->InitializeDevice());

    // Create the rendering context
    YDS_NESTED_ERROR_CALL(m_device->CreateRenderingContext(&m_renderingContext, m_gameWindow));

    m_windowHandler.Initialize(m_device, m_renderingContext, this);

    // Main render target
    YDS_NESTED_ERROR_CALL(m_device->CreateOnScreenRenderTarget(&m_mainRenderTarget, m_renderingContext, depthBuffer));

    // Initialize Geometry
    YDS_NESTED_ERROR_CALL(InitializeGeometry());

    // Initialize the console
    m_console.SetEngine(this);
    YDS_NESTED_ERROR_CALL(m_console.Initialize());

    // Initialize Shaders
    YDS_NESTED_ERROR_CALL(InitializeShaders(shaderDirectory));

    // Timing System
    m_timingSystem = ysTimingSystem::Get();
    m_timingSystem->Initialize();

    m_initialized = true;

    SetWindowSize(m_gameWindow->GetScreenWidth(), m_gameWindow->GetScreenHeight());

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::StartFrame() {
    YDS_ERROR_DECLARE("StartFrame");

    m_windowSystem->ProcessMessages();
    m_timingSystem->Update();

    if (IsOpen()) {
        m_device->SetRenderTarget(m_mainRenderTarget);
        m_device->ClearBuffers(m_clearColor);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::EndFrame() {
    YDS_ERROR_DECLARE("EndFrame");

    if (IsOpen()) {
        ExecuteDrawQueue(DrawTarget::Main);
        ExecuteDrawQueue(DrawTarget::Gui);
        m_device->Present();
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    YDS_NESTED_ERROR_CALL(m_device->DestroyRenderTarget(m_mainRenderTarget));
    YDS_NESTED_ERROR_CALL(m_device->DestroyRenderingContext(m_renderingContext));

    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_mainIndexBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_mainVertexBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_shaderObjectVariablesBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_shaderScreenVariablesBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_shaderSkinningControlsBuffer));

    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_vertexShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_vertexSkinnedShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShaderProgram(m_shaderProgram));

    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_inputLayout));
    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_skinnedInputLayout));

    YDS_NESTED_ERROR_CALL(m_device->DestroyDevice());
    m_mainKeyboard = nullptr;
    m_mainMouse = nullptr;

    YDS_NESTED_ERROR_CALL(ysInputSystem::DestroyInputSystem(m_inputSystem));

    m_windowSystem->DeleteAllWindows();
    m_gameWindow = nullptr;

    ysWindowSystem::DestroyWindowSystem(m_windowSystem);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::UseMaterial(Material *material) {
    YDS_ERROR_DECLARE("UseMaterial");

    if (material == nullptr) {
        SetMultiplyColor(ysVector4(1.0f, 0.0f, 1.0f, 1.0f));
    }
    else {
        SetMultiplyColor(material->GetDiffuseColor());
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::InitializeGeometry() {
    YDS_ERROR_DECLARE("InitializeGeometry");

    Vertex vertexData[] = {
        { { -1.0f, 1.0f, -1.0f, 1.0f },		{0.0f, 1.0f},	{0.0f, 0.0f, 1.0f, 0.0f} },
        { { 1.0f, 1.0f, -1.0f, 1.0f },		{1.0f, 1.0f},	{0.0f, 0.0f, 1.0f, 0.0f} },
        { { 1.0f, -1.0f, -1.0f, 1.0f },		{1.0f, 0.0f},	{0.0f, 0.0f, 1.0f, 0.0f} },
        { { -1.0f, -1.0f, -1.0f, 1.0f },    {0.0f, 0.0f},	{0.0f, 0.0f, 1.0f, 0.0f} } };

    unsigned short indices[] = {
        2, 1, 0,
        3, 2, 0 };

    YDS_NESTED_ERROR_CALL(m_device->CreateVertexBuffer(&m_mainVertexBuffer, sizeof(vertexData), (char *)vertexData));
    YDS_NESTED_ERROR_CALL(m_device->CreateIndexBuffer(&m_mainIndexBuffer, sizeof(indices), (char *)indices));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::InitializeShaders(const char *shaderDirectory) {
    YDS_ERROR_DECLARE("InitializeShaders");

    char buffer[256];

    if (m_device->GetAPI() == ysContextObject::DIRECTX11 || m_device->GetAPI() == ysContextObject::DIRECTX10) {
        sprintf_s(buffer, "%s%s", shaderDirectory, "delta_engine_shader.fx");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexShader, buffer, "VS_STANDARD"));
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexSkinnedShader, buffer, "VS_SKINNED"));
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_pixelShader, buffer, "PS"));
    }
    else if (m_device->GetAPI() == ysContextObject::OPENGL4_0) {
        sprintf_s(buffer, "%s%s", shaderDirectory, "delta_engine_shader.glvs");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexShader, buffer, "VS"));

        sprintf_s(buffer, "%s%s", shaderDirectory, "delta_engine_shader.glvs");
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_pixelShader, buffer, "PS"));
    }

    m_skinnedFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT);
    m_skinnedFormat.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32_FLOAT);
    m_skinnedFormat.AddChannel("NORMAL", sizeof(float) * (4 + 2), ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT);
    m_skinnedFormat.AddChannel("BONE_INDICES", sizeof(float) * (4 + 2 + 4), ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_UINT);
    m_skinnedFormat.AddChannel("BONE_WEIGHTS", sizeof(float) * (4 + 2 + 4) + sizeof(int) * 4, ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT);

    m_standardFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT);
    m_standardFormat.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32_FLOAT);
    m_standardFormat.AddChannel("NORMAL", sizeof(float) * (4 + 2), ysRenderGeometryChannel::CHANNEL_FORMAT_R32G32B32A32_FLOAT);

    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_inputLayout, m_vertexShader, &m_standardFormat));
    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_skinnedInputLayout, m_vertexSkinnedShader, &m_skinnedFormat));

    YDS_NESTED_ERROR_CALL(m_device->CreateShaderProgram(&m_shaderProgram));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_shaderProgram, m_vertexShader));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_shaderProgram, m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->LinkProgram(m_shaderProgram));

    YDS_NESTED_ERROR_CALL(m_device->CreateShaderProgram(&m_skinnedShaderProgram));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_skinnedShaderProgram, m_vertexSkinnedShader));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_skinnedShaderProgram, m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->LinkProgram(m_skinnedShaderProgram));

    // Create shader controls
    YDS_NESTED_ERROR_CALL(m_device->CreateConstantBuffer(&m_shaderObjectVariablesBuffer, sizeof(ShaderObjectVariables), nullptr));
    YDS_NESTED_ERROR_CALL(m_device->CreateConstantBuffer(&m_shaderScreenVariablesBuffer, sizeof(ShaderScreenVariables), nullptr));
    YDS_NESTED_ERROR_CALL(m_device->CreateConstantBuffer(&m_shaderSkinningControlsBuffer, sizeof(ShaderSkinningControls), nullptr));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::LoadTexture(ysTexture **image, const char *fname) {
    YDS_ERROR_DECLARE("LoadTexture");

    YDS_NESTED_ERROR_CALL(m_device->CreateTexture(image, fname));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::LoadAnimation(Animation **animation, const char *path, int start, int end) {
    YDS_ERROR_DECLARE("LoadAnimation");

    ysTexture **list = new ysTexture * [end - start + 1];
    char buffer[256];
    for (int i = start; i <= end; i++) {
        sprintf_s(buffer, 256, "%s/%.4i.png", path, i);

        YDS_NESTED_ERROR_CALL(m_device->CreateTexture(&list[i - start], buffer));
    }

    Animation *newAnimation;
    newAnimation = new Animation;
    newAnimation->m_nFrames = end - start + 1;
    newAnimation->m_textures = list;

    *animation = newAnimation;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void dbasic::DeltaEngine::SubmitSkeleton(Skeleton *skeleton) {
    int nBones = skeleton->GetBoneCount();

    for (int i = 0; i < nBones; i++) {
        Bone *bone = skeleton->GetBone(i);
        m_shaderSkinningControls.BoneTransforms[i] = ysMath::Transpose(skeleton->GetBone(i)->GetSkinMatrix());
    }
}

void dbasic::DeltaEngine::SetCameraPosition(float x, float y) {
    m_cameraX = x;
    m_cameraY = y;

    m_shaderScreenVariablesSync = false;
}

void dbasic::DeltaEngine::SetCameraAngle(float angle) {
    m_cameraAngle = angle;
    m_shaderScreenVariablesSync = false;
}

void dbasic::DeltaEngine::SetCameraAltitude(float altitude) {
    m_cameraAltitude = altitude;
    m_shaderScreenVariablesSync = false;
}

void dbasic::DeltaEngine::SetObjectTransform(const ysMatrix &mat) {
    m_shaderObjectVariablesSync = false;
    m_shaderObjectVariables.Transform = mat;
}

void dbasic::DeltaEngine::SetPositionOffset(const ysVector &position) {
    m_shaderObjectVariablesSync = false;
    m_shaderObjectVariables.Transform = ysMath::MatMult(m_shaderObjectVariables.Transform, ysMath::TranslationTransform(position));
}

void dbasic::DeltaEngine::SetWindowSize(int width, int height) {
    m_shaderScreenVariablesSync = false;
}

void dbasic::DeltaEngine::SetClearColor(int r, int g, int b) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;

    m_clearColor[0] = fr;
    m_clearColor[1] = fg;
    m_clearColor[2] = fb;
    m_clearColor[3] = 1.0f;
}

bool dbasic::DeltaEngine::IsKeyDown(ysKeyboard::KEY_CODE key) {
    if (m_mainKeyboard != nullptr) {
        ysKeyboard *keyboard = m_mainKeyboard->GetAsKeyboard();
        return keyboard->GetKey(key)->IsDown();
    }

    return false;
}

bool dbasic::DeltaEngine::ProcessKeyDown(ysKeyboard::KEY_CODE key) {
    if (m_mainKeyboard != nullptr) {
        ysKeyboard *keyboard = m_mainKeyboard->GetAsKeyboard();
        return keyboard->ProcessKeyTransition(key);
    }

    return false;
}

bool dbasic::DeltaEngine::IsMouseKeyDown(ysMouse::BUTTON_CODE key) {
    if (m_mainMouse != nullptr) {
        ysMouse *mouse = m_mainMouse->GetAsMouse();
        return mouse->GetButton(key)->IsDown();
    }

    return false;
}

int dbasic::DeltaEngine::GetMouseWheel() {
    if (m_mainMouse != nullptr) {
        ysMouse *mouse = m_mainMouse->GetAsMouse();
        return mouse->GetWheel();
    }

    return 0;
}

void dbasic::DeltaEngine::GetMousePos(int *x, int *y) {
    if (m_mainMouse != nullptr) {
        ysMouse *mouse = m_mainMouse->GetAsMouse();

        if (x != nullptr) {
            *x = mouse->GetX();
        }

        if (y != nullptr) {
            *y = mouse->GetY();
        }
    }
}

float dbasic::DeltaEngine::GetFrameLength() {
    return (float)(m_timingSystem->GetFrameDuration());
}

float dbasic::DeltaEngine::GetAverageFramerate() {
    return m_timingSystem->GetFPS();
}

void dbasic::DeltaEngine::SetMultiplyColor(ysVector4 color) {
    m_shaderObjectVariables.MulCol = color;
}

void dbasic::DeltaEngine::ResetMultiplyColor() {
    m_shaderObjectVariables.MulCol = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

int dbasic::DeltaEngine::GetScreenWidth() {
    return m_gameWindow->GetWidth();
}

int dbasic::DeltaEngine::GetScreenHeight() {
    return m_gameWindow->GetHeight();
}

ysError dbasic::DeltaEngine::DrawImage(ysTexture *image, int layer, float scaleX, float scaleY, float texOffsetU, float texOffsetV, float texScaleU, float texScaleV) {
    YDS_ERROR_DECLARE("DrawImage");

    if (!m_shaderObjectVariablesSync) {
        //m_shaderObjectVariables.MulCol.x = 1.0f;
        //m_shaderObjectVariables.MulCol.y = 1.0f;
        //m_shaderObjectVariables.MulCol.z = 1.0f;
        //m_shaderObjectVariables.MulCol.w = 1.0f;

        m_shaderObjectVariables.Scale[0] = scaleX;
        m_shaderObjectVariables.Scale[1] = scaleY;
        m_shaderObjectVariables.Scale[2] = 1.0f;

        m_shaderObjectVariables.TexOffset[0] = texOffsetU;
        m_shaderObjectVariables.TexOffset[1] = texOffsetV;

        m_shaderObjectVariables.TexScale[0] = texScaleU;
        m_shaderObjectVariables.TexScale[1] = texScaleV;

        m_shaderObjectVariables.ColorReplace = 0;
        m_shaderObjectVariables.Lit = 0;
    }

    DrawCall *newCall = nullptr;
    if (m_currentTarget == DrawTarget::Main) {
        newCall = &m_drawQueue[layer].New();
    }
    else if (m_currentTarget == DrawTarget::Gui) {
        newCall = &m_drawQueueGui[layer].New();
    }

    if (newCall != nullptr) {
        newCall->ObjectVariables = m_shaderObjectVariables;
        newCall->Texture = image;
        newCall->Model = nullptr;
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::DrawBox(const int color[3], float width, float height, int layer) {
    YDS_ERROR_DECLARE("DrawBox");

    if (!m_shaderObjectVariablesSync) {
        m_shaderObjectVariables.Scale[0] = (float)width / 2.0f;
        m_shaderObjectVariables.Scale[1] = (float)height / 2.0f;
        m_shaderObjectVariables.Scale[2] = 1.0f;

        m_shaderObjectVariables.TexOffset[0] = 0.0f;
        m_shaderObjectVariables.TexOffset[1] = 0.0f;

        m_shaderObjectVariables.TexScale[0] = 1.0f;
        m_shaderObjectVariables.TexScale[1] = 1.0f;

        m_shaderObjectVariables.ColorReplace = 1;
        m_shaderObjectVariables.MulCol.x = color[0] / 255.0f;
        m_shaderObjectVariables.MulCol.y = color[1] / 255.0f;
        m_shaderObjectVariables.MulCol.z = color[2] / 255.0f;
        m_shaderObjectVariables.MulCol.w = 1.0f;

        m_shaderObjectVariables.Lit = 0;
    }

    DrawCall *newCall = nullptr;
    if (m_currentTarget == DrawTarget::Main) {
        newCall = &m_drawQueue[layer].New();
    }
    else if (m_currentTarget == DrawTarget::Gui) {
        newCall = &m_drawQueueGui[layer].New();
    }

    if (newCall != nullptr) {
        newCall->ObjectVariables = m_shaderObjectVariables;
        newCall->Texture = nullptr;
        newCall->Model = nullptr;
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::DrawAxis(const int color[3], const ysVector &position, const ysVector &direction, float width, float length, int layer) {
    YDS_ERROR_DECLARE("DrawAxis");

    ysMatrix trans = ysMath::TranslationTransform(position);
    ysMatrix offset = ysMath::TranslationTransform(ysMath::LoadVector(0, length / 2.0f));

    ysVector orth = ysMath::Cross(direction, ysMath::Constants::ZAxis);
    ysMatrix dir = ysMath::LoadMatrix(orth, direction, ysMath::Constants::ZAxis, ysMath::Constants::IdentityRow4);
    dir = ysMath::Transpose(dir);

    ysMatrix transform = ysMath::MatMult(trans, dir);
    transform = ysMath::MatMult(transform, offset);
    SetObjectTransform(transform);

    YDS_NESTED_ERROR_CALL(DrawBox(color, width, length, layer));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::DrawModel(ModelAsset *model, const ysMatrix &transform, float scale, ysTexture *texture, int layer) {
    YDS_ERROR_DECLARE("DrawAxis");

    if (!m_shaderObjectVariablesSync) {
        m_shaderObjectVariables.Transform = transform;
        m_shaderObjectVariables.Scale[0] = scale;
        m_shaderObjectVariables.Scale[1] = scale;
        m_shaderObjectVariables.Scale[2] = scale;

        m_shaderObjectVariables.TexOffset[0] = 0.0f;
        m_shaderObjectVariables.TexOffset[1] = 0.0f;

        m_shaderObjectVariables.TexScale[0] = 1.0f;
        m_shaderObjectVariables.TexScale[1] = 1.0f;

        m_shaderObjectVariables.ColorReplace = 1;
        m_shaderObjectVariables.Lit = 1;

        //m_shaderObjectVariables.ColorReplace = 1;
        //m_shaderObjectVariables.MulCol.x = 1.0f;
        //m_shaderObjectVariables.MulCol.y = 1.0f;
        //m_shaderObjectVariables.MulCol.z = 1.0f;
        //m_shaderObjectVariables.MulCol.w = 1.0f;
    }

    DrawCall *newCall = nullptr;
    if (m_currentTarget == DrawTarget::Main) {
        newCall = &m_drawQueue[layer].New();
    }
    else if (m_currentTarget == DrawTarget::Gui) {
        newCall = &m_drawQueueGui[layer].New();
    }

    if (newCall != nullptr) {
        newCall->ObjectVariables = m_shaderObjectVariables;
        newCall->Texture = texture;
        newCall->Model = model;
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::DrawRenderSkeleton(RenderSkeleton *skeleton, float scale, int layer) {
    YDS_ERROR_DECLARE("DrawRenderSkeleton");

    int nodeCount = skeleton->GetNodeCount();
    for (int i = 0; i < nodeCount; ++i) {
        RenderNode *node = skeleton->GetNode(i);
        if (node->GetModelAsset() != nullptr) {
            Material *material = node->GetModelAsset()->GetMaterial();
            UseMaterial(node->GetModelAsset()->GetMaterial());

            ysTexture *diffuseMap = material == nullptr
                ? nullptr
                : material->GetDiffuseMap();

            DrawModel(
                node->GetModelAsset(),
                node->RigidBody.GetTransform(),
                scale,
                diffuseMap,
                layer);
        }
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::DeltaEngine::ExecuteDrawQueue(DrawTarget target) {
    YDS_ERROR_DECLARE("ExecuteDrawQueue");

    YDS_NESTED_ERROR_CALL(m_device->EditBufferData(m_shaderSkinningControlsBuffer, (char *)(&m_shaderSkinningControls)));

    if (!m_shaderScreenVariablesSync) {
        float aspect = m_gameWindow->GetScreenWidth() / (float)m_gameWindow->GetScreenHeight();

        if (target == DrawTarget::Main) {
            m_shaderScreenVariables.Projection = ysMath::Transpose(ysMath::FrustrumPerspective(ysMath::Constants::PI / 3.0f, aspect, 1.0f, 10000.0f));
        }
        else if (target == DrawTarget::Gui) {
            m_shaderScreenVariables.Projection = ysMath::Transpose(
                ysMath::OrthographicProjection(
                    (float)m_gameWindow->GetScreenWidth(), 
                    (float)m_gameWindow->GetScreenHeight(), 
                    0.001f, 
                    500.0f));
        }

        float sinRot = sin(m_cameraAngle * ysMath::Constants::PI / 180.0f);
        float cosRot = cos(m_cameraAngle * ysMath::Constants::PI / 180.0f);

        ysVector cameraEye;
        ysVector cameraTarget;

        if (target == DrawTarget::Main) {
            cameraEye = ysMath::LoadVector(m_cameraX, m_cameraY, m_cameraAltitude, 1.0f);
        }
        else if (target == DrawTarget::Gui) {
            cameraEye = ysMath::LoadVector(0.0f, 0.0f, m_cameraAltitude, 1.0f);
        }

        ysVector up = ysMath::LoadVector(-sinRot, cosRot);

        if (target == DrawTarget::Main) {
            cameraTarget = ysMath::LoadVector(m_cameraX, m_cameraY, 0.0f, 1.0f);
        }
        else if (target == DrawTarget::Gui) {
            cameraTarget = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 1.0f);
        }

        m_shaderScreenVariables.CameraView = ysMath::Transpose(ysMath::CameraTarget(cameraEye, cameraTarget, up));

        m_shaderScreenVariables.Eye[0] = m_cameraX;
        m_shaderScreenVariables.Eye[1] = m_cameraY;
        m_shaderScreenVariables.Eye[2] = m_cameraAltitude;

        YDS_NESTED_ERROR_CALL(m_device->EditBufferData(m_shaderScreenVariablesBuffer, (char *)(&m_shaderScreenVariables)));

        // NOTE - Update the screen variables each frame so sync is not set to true
        // after the update happens.
        m_shaderScreenVariablesSync = false;
    }

    for (int i = 0; i < MAX_LAYERS; i++) {
        int objectsAtLayer = 0;

        if (target == DrawTarget::Main) objectsAtLayer = m_drawQueue[i].GetNumObjects();
        else if (target == DrawTarget::Gui) objectsAtLayer = m_drawQueueGui[i].GetNumObjects();

        for (int j = 0; j < objectsAtLayer; j++) {
            DrawCall *call = nullptr;
            if (target == DrawTarget::Main) call = &m_drawQueue[i][j];
            else if (target == DrawTarget::Gui) call = &m_drawQueueGui[i][j];

            if (call == nullptr) continue;

            m_device->EditBufferData(m_shaderObjectVariablesBuffer, (char *)(&call->ObjectVariables));

            if (call->Model != nullptr) {
                m_device->SetDepthTestEnabled(m_mainRenderTarget, true);

                if (call->Model->GetBoneCount() <= 0) {
                    m_device->UseInputLayout(m_inputLayout);
                    m_device->UseShaderProgram(m_shaderProgram);
                }
                else {
                    m_device->UseInputLayout(m_skinnedInputLayout);
                    m_device->UseShaderProgram(m_skinnedShaderProgram);
                }

                m_device->UseConstantBuffer(m_shaderScreenVariablesBuffer, 0);
                m_device->UseConstantBuffer(m_shaderObjectVariablesBuffer, 1);
                m_device->UseConstantBuffer(m_shaderSkinningControlsBuffer, 2);

                m_device->UseTexture(call->Texture, 0);

                m_device->UseIndexBuffer(call->Model->GetIndexBuffer(), 0);
                m_device->UseVertexBuffer(call->Model->GetVertexBuffer(), call->Model->GetVertexSize(), 0);

                m_device->Draw(call->Model->GetFaceCount(), call->Model->GetBaseIndex(), call->Model->GetBaseVertex());
            }
            else {
                m_device->SetDepthTestEnabled(m_mainRenderTarget, false);

                m_device->UseInputLayout(m_inputLayout);
                m_device->UseShaderProgram(m_shaderProgram);

                m_device->UseConstantBuffer(m_shaderScreenVariablesBuffer, 0);
                m_device->UseConstantBuffer(m_shaderObjectVariablesBuffer, 1);
                m_device->UseConstantBuffer(m_shaderSkinningControlsBuffer, 2);

                m_device->UseIndexBuffer(m_mainIndexBuffer, 0);
                m_device->UseVertexBuffer(m_mainVertexBuffer, sizeof(Vertex), 0);

                m_device->UseTexture(call->Texture, 0);

                m_device->Draw(2, 0, 0);
            }
        }

        if (target == DrawTarget::Gui) {
            m_drawQueueGui[i].Clear();
        }
        else if (target == DrawTarget::Main) {
            m_drawQueue[i].Clear();
        }
    }

    if (target == DrawTarget::Gui) {
        ShaderObjectVariables objectSettings;
        objectSettings.ColorReplace = 0;
        objectSettings.Lit = 0;
        objectSettings.MulCol = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);
        objectSettings.Scale[0] = 1.0f;
        objectSettings.Scale[1] = 1.0f;
        objectSettings.Scale[2] = 1.0f;
        objectSettings.TexOffset[0] = 0.0f;
        objectSettings.TexOffset[1] = 0.0f;
        objectSettings.TexScale[0] = 1.0f;
        objectSettings.TexScale[1] = 1.0f;
        objectSettings.Transform = ysMath::LoadIdentity();

        m_device->EditBufferData(m_shaderObjectVariablesBuffer, (char *)(&objectSettings));

        m_device->UseInputLayout(m_inputLayout);
        m_device->UseShaderProgram(m_shaderProgram);

        m_device->UseConstantBuffer(m_shaderScreenVariablesBuffer, 0);
        m_device->UseConstantBuffer(m_shaderObjectVariablesBuffer, 1);
        m_device->UseConstantBuffer(m_shaderSkinningControlsBuffer, 2);

        YDS_NESTED_ERROR_CALL(m_console.UpdateDisplay());
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
