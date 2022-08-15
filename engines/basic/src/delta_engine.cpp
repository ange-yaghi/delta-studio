#include "../include/delta_engine.h"

#include "../include/skeleton.h"
#include "../include/render_skeleton.h"
#include "../include/material.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION

#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#include <assert.h>

const std::string dbasic::DeltaEngine::FrameBreakdownFull = "Frame Full";
const std::string dbasic::DeltaEngine::FrameBreakdownRenderScene = "Scene";
const dbasic::DeltaEngine::GameEngineSettings dbasic::DeltaEngine::DefaultSettings;

dbasic::DeltaEngine::DeltaEngine() {
    m_device = nullptr;

    m_audioDevice = nullptr;

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
    m_shaderSet = nullptr;

    m_consolePixelShader = nullptr;
    m_consoleVertexShader = nullptr;

    m_consoleProgram = nullptr;
    m_skinnedShaderProgram = nullptr;

    m_skinnedInputLayout = nullptr;
    m_inputLayout = nullptr;
    m_consoleInputLayout = nullptr;
    m_saqInputLayout = nullptr;

    m_initialized = false;

    m_clearColor[0] = 0.0F;
    m_clearColor[1] = 0.0F;
    m_clearColor[2] = 0.0F;
    m_clearColor[3] = 1.0F;

    // Input system
    m_mainKeyboard = nullptr;
    m_inputSystem = nullptr;
    m_mainMouse = nullptr;

    m_drawQueue = new ysExpandingArray<DrawCall, 256>[MaxLayers];

    m_cursorHidden = false;
    m_cursorPositionLocked = false;
}

dbasic::DeltaEngine::~DeltaEngine() {
    assert(m_windowSystem == nullptr);
    assert(m_gameWindow == nullptr);
    assert(m_audioSystem == nullptr);
    assert(m_audioDevice == nullptr);
    assert(m_mainVertexBuffer == nullptr);
    assert(m_mainIndexBuffer == nullptr);
    assert(m_vertexShader == nullptr);
    assert(m_vertexSkinnedShader == nullptr);
    assert(m_consoleVertexShader == nullptr);
    assert(m_saqVertexShader == nullptr);
    assert(m_pixelShader == nullptr);
    assert(m_saqPixelShader == nullptr);
    assert(m_consolePixelShader == nullptr);
    assert(m_shaderProgram == nullptr);
    assert(m_consoleProgram == nullptr);
    assert(m_skinnedShaderProgram == nullptr);
    assert(m_inputLayout == nullptr);
    assert(m_skinnedInputLayout == nullptr);
    assert(m_consoleInputLayout == nullptr);
    assert(m_saqInputLayout == nullptr);
    assert(m_mainKeyboard == nullptr);
    assert(m_inputSystem == nullptr);
    assert(m_mainMouse == nullptr);
    assert(m_consoleShaderObjectVariablesBuffer == nullptr);

    delete[] m_drawQueue;
}

ysError dbasic::DeltaEngine::CreateGameWindow(const GameEngineSettings &settings) {
    YDS_ERROR_DECLARE("CreateGameWindow");

    // Create the window system
    YDS_NESTED_ERROR_CALL(ysWindowSystem::CreateWindowSystem(&m_windowSystem, ysWindowSystemObject::Platform::Sdl));
    m_windowSystem->ConnectInstance(settings.Instance);

    // Find the monitor setup
    m_windowSystem->SurveyMonitors();
    ysMonitor *mainMonitor = m_windowSystem->GetMonitor(0);

    // Create the game window
    YDS_NESTED_ERROR_CALL(m_windowSystem->NewWindow(&m_gameWindow));
    YDS_NESTED_ERROR_CALL(m_gameWindow->InitializeWindow(
        nullptr,
        settings.WindowTitle,
        settings.WindowStyle,
        settings.WindowPositionX, settings.WindowPositionY,
        settings.WindowWidth, settings.WindowHeight,
        mainMonitor));
    m_gameWindow->AttachEventHandler(&m_windowHandler);

    YDS_NESTED_ERROR_CALL(ysInputSystem::CreateInputSystem(&m_inputSystem, ysWindowSystemObject::Platform::Windows));
    m_windowSystem->AssignInputSystem(m_inputSystem);
    m_inputSystem->Initialize();

    m_mainKeyboard = m_inputSystem->GetKeyboardAggregator();
    m_mainMouse = m_inputSystem->GetMouseAggregator();

    // Create the graphics device
    YDS_NESTED_ERROR_CALL(ysDevice::CreateDevice(&m_device, settings.API));
    YDS_NESTED_ERROR_CALL(m_device->InitializeDevice());

    // Create the audio device
    YDS_NESTED_ERROR_CALL(ysAudioSystem::CreateAudioSystem(&m_audioSystem, ysAudioSystem::API::DirectSound8));
    m_audioSystem->EnumerateDevices();
    m_audioDevice = m_audioSystem->GetPrimaryDevice();
    m_audioSystem->ConnectDevice(m_audioDevice, m_gameWindow);

    // Create the rendering context
    YDS_NESTED_ERROR_CALL(m_device->CreateRenderingContext(&m_renderingContext, m_gameWindow));

    m_windowHandler.Initialize(m_device, m_renderingContext, this);

    // Main render target
    YDS_NESTED_ERROR_CALL(m_device->CreateOnScreenRenderTarget(&m_mainRenderTarget, m_renderingContext, settings.DepthBuffer));

    m_mainRenderTarget->SetDebugName("MAIN_RENDER_TARGET");

    // Initialize Geometry
    YDS_NESTED_ERROR_CALL(InitializeGeometry());

    // Initialize UI renderer
    m_uiRenderer.SetEngine(this);
    YDS_NESTED_ERROR_CALL(m_uiRenderer.Initialize(32768));

    // Initialize the console
    m_console.SetEngine(this);
    m_console.SetRenderer(&m_uiRenderer);
    YDS_NESTED_ERROR_CALL(m_console.Initialize());

    // Initialize Shaders
    YDS_NESTED_ERROR_CALL(InitializeShaders(settings.ShaderDirectory));

    // Timing System
    m_timingSystem = ysTimingSystem::Get();
    m_timingSystem->Initialize();

    InitializeBreakdownTimer(settings.LoggingDirectory);

    m_initialized = true;

    m_windowHandler.OnResizeWindow(m_gameWindow->GetWidth(), m_gameWindow->GetHeight());

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::StartFrame() {
    YDS_ERROR_DECLARE("StartFrame");

    m_uiRenderer.Reset();

    m_breakdownTimer.WriteLastFrameToLogFile();
    m_breakdownTimer.StartFrame();
    m_breakdownTimer.StartMeasurement(FrameBreakdownFull);

    m_audioDevice->UpdateAudioSources();
    m_windowSystem->ProcessMessages();
    m_timingSystem->Update();

    // TEMP
    if (IsKeyDown(ysKey::Code::B)) m_device->SetDebugFlag(0, true);
    else m_device->SetDebugFlag(0, false);

    if (m_gameWindow->IsActive()) {
        m_windowSystem->SetCursorVisible(!m_cursorHidden);
        if (m_cursorPositionLocked) {
            m_windowSystem->ReleaseCursor();
            m_windowSystem->ConfineCursor(m_gameWindow);
        }
    }
    else {
        m_windowSystem->SetCursorVisible(true);
        m_windowSystem->ReleaseCursor();
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::EndFrame() {
    YDS_ERROR_DECLARE("EndFrame");

    if (IsOpen()) {
        if (IsConsoleEnabled()) {
            YDS_NESTED_ERROR_CALL(m_console.UpdateGeometry());
        }

        YDS_NESTED_ERROR_CALL(m_uiRenderer.UpdateDisplay());

        m_breakdownTimer.StartMeasurement(FrameBreakdownRenderScene);
        {
            YDS_NESTED_ERROR_CALL(ExecuteDrawQueue());
        }
        m_breakdownTimer.EndMeasurement(FrameBreakdownRenderScene);

        ClearDrawQueue();

        YDS_NESTED_ERROR_CALL(m_device->Present());
    }
    else {
        m_breakdownTimer.SkipMeasurement(FrameBreakdownRenderScene);
    }

    m_breakdownTimer.EndMeasurement(FrameBreakdownFull);
    m_breakdownTimer.EndFrame();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    YDS_NESTED_ERROR_CALL(GetConsole()->Destroy());
    YDS_NESTED_ERROR_CALL(GetUiRenderer()->Destroy());

    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_mainIndexBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_mainVertexBuffer));
    YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_consoleShaderObjectVariablesBuffer));

    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_vertexShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_vertexSkinnedShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_consoleVertexShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_saqVertexShader));

    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_saqPixelShader));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShader(m_consolePixelShader));

    YDS_NESTED_ERROR_CALL(m_device->DestroyShaderProgram(m_shaderProgram));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShaderProgram(m_consoleProgram));
    YDS_NESTED_ERROR_CALL(m_device->DestroyShaderProgram(m_skinnedShaderProgram));

    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_inputLayout));
    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_skinnedInputLayout));
    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_consoleInputLayout));
    YDS_NESTED_ERROR_CALL(m_device->DestroyInputLayout(m_saqInputLayout));

    YDS_NESTED_ERROR_CALL(m_device->DestroyRenderTarget(m_mainRenderTarget));
    YDS_NESTED_ERROR_CALL(m_device->DestroyRenderingContext(m_renderingContext));

    YDS_NESTED_ERROR_CALL(m_device->DestroyDevice());

    m_audioSystem->DisconnectDevice(m_audioDevice);
    ysAudioSystem::DestroyAudioSystem(&m_audioSystem);

    m_audioDevice = nullptr;

    m_mainKeyboard = nullptr;
    m_mainMouse = nullptr;

    YDS_NESTED_ERROR_CALL(ysInputSystem::DestroyInputSystem(m_inputSystem));

    m_windowSystem->DeleteAllWindows();
    m_gameWindow = nullptr;

    ysWindowSystem::DestroyWindowSystem(m_windowSystem);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::InitializeShaderSet(ShaderSet *shaderSet) {
    YDS_ERROR_DECLARE("InitializeShaderSet");

    YDS_NESTED_ERROR_CALL(shaderSet->Initialize(m_device));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::InitializeDefaultShaders(DefaultShaders *shaders, ShaderSet *shaderSet) {
    YDS_ERROR_DECLARE("InitializeDefaultShaders");

    YDS_NESTED_ERROR_CALL(shaders->Initialize(shaderSet, m_mainRenderTarget, m_shaderProgram, m_inputLayout));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::InitializeConsoleShaders(ShaderSet *shaderSet) {
    YDS_ERROR_DECLARE("InitializeConsoleShaders");

    YDS_NESTED_ERROR_CALL(
        m_uiRenderer.GetShaders()->Initialize(shaderSet, m_mainRenderTarget, m_consoleProgram, m_consoleInputLayout));

    return YDS_ERROR_RETURN(ysError::None);
}

dbasic::DeltaEngine::DrawCall *dbasic::DeltaEngine::NewDrawCall(int layer, int objectDataSize) {
    DrawCall *newCall = &m_drawQueue[layer].New();
    if (newCall != nullptr) {
        newCall->ObjectData = malloc(objectDataSize);
        newCall->ObjectDataSize = objectDataSize;
    }

    return newCall;
}

ysError dbasic::DeltaEngine::InitializeGeometry() {
    YDS_ERROR_DECLARE("InitializeGeometry");

    Vertex vertexData[] = {
        { { -1.0f, 1.0f, 0.0f, 1.0f },  {0.0f, 1.0f},    {0.0f, 0.0f, 1.0f, 0.0f} },
        { { 1.0f, 1.0f, 0.0f, 1.0f },   {1.0f, 1.0f},    {0.0f, 0.0f, 1.0f, 0.0f} },
        { { 1.0f, -1.0f, 0.0f, 1.0f },  {1.0f, 0.0f},    {0.0f, 0.0f, 1.0f, 0.0f} },
        { { -1.0f, -1.0f, 0.0f, 1.0f }, {0.0f, 0.0f},    {0.0f, 0.0f, 1.0f, 0.0f} } };

    unsigned short indices[] = {
        2, 1, 0,
        3, 2, 0 };

    YDS_NESTED_ERROR_CALL(m_device->CreateVertexBuffer(&m_mainVertexBuffer, sizeof(vertexData), (char *)vertexData));
    YDS_NESTED_ERROR_CALL(m_device->CreateIndexBuffer(&m_mainIndexBuffer, sizeof(indices), (char *)indices));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::InitializeShaders(const char *shaderDirectory) {
    YDS_ERROR_DECLARE("InitializeShaders");

    char buffer[256];

    if (m_device->GetAPI() == ysContextObject::DeviceAPI::DirectX11 ||
        m_device->GetAPI() == ysContextObject::DeviceAPI::DirectX10)
    {
        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/hlsl/delta_engine_shader.fx");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexShader, buffer, "VS_STANDARD"));
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexSkinnedShader, buffer, "VS_SKINNED"));
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_pixelShader, buffer, "PS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/hlsl/delta_console_shader.fx");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_consoleVertexShader, buffer, "VS_CONSOLE"));
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_consolePixelShader, buffer, "PS_CONSOLE"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/hlsl/delta_saq_shader.fx");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_saqVertexShader, buffer, "VS_SAQ"));
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_saqPixelShader, buffer, "PS_SAQ"));
    }
    else if (m_device->GetAPI() == ysContextObject::DeviceAPI::OpenGL4_0) {
        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_engine_shader.vert");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexShader, buffer, "VS"));
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_vertexSkinnedShader, buffer, "VS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_engine_shader.frag");
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_pixelShader, buffer, "PS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_console_shader.vert");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_consoleVertexShader, buffer, "VS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_console_shader.frag");
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_consolePixelShader, buffer, "PS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_saq_shader.vert");
        YDS_NESTED_ERROR_CALL(m_device->CreateVertexShader(&m_saqVertexShader, buffer, "VS"));

        sprintf_s(buffer, 256, "%s%s", shaderDirectory, "/glsl/delta_saq_shader.frag");
        YDS_NESTED_ERROR_CALL(m_device->CreatePixelShader(&m_saqPixelShader, buffer, "PS"));
    }

    m_skinnedFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_skinnedFormat.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_skinnedFormat.AddChannel("NORMAL", sizeof(float) * (4 + 2), ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_skinnedFormat.AddChannel("BONE_INDICES", sizeof(float) * (4 + 2 + 4), ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_UINT);
    m_skinnedFormat.AddChannel("BONE_WEIGHTS", sizeof(float) * (4 + 2 + 4) + sizeof(int) * 4, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);

    m_standardFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_standardFormat.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_standardFormat.AddChannel("NORMAL", sizeof(float) * (4 + 2), ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);

    m_consoleVertexFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_consoleVertexFormat.AddChannel("TEXCOORD", sizeof(float) * 2, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_consoleVertexFormat.AddChannel("COLOR", sizeof(float) * (2 + 2), ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);

    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_inputLayout, m_vertexShader, &m_standardFormat));
    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_skinnedInputLayout, m_vertexSkinnedShader, &m_skinnedFormat));
    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_consoleInputLayout, m_consoleVertexShader, &m_consoleVertexFormat));
    YDS_NESTED_ERROR_CALL(m_device->CreateInputLayout(&m_saqInputLayout, m_saqVertexShader, &m_standardFormat));

    YDS_NESTED_ERROR_CALL(m_device->CreateShaderProgram(&m_shaderProgram));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_shaderProgram, m_vertexShader));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_shaderProgram, m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->LinkProgram(m_shaderProgram));

    YDS_NESTED_ERROR_CALL(m_device->CreateShaderProgram(&m_skinnedShaderProgram));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_skinnedShaderProgram, m_vertexSkinnedShader));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_skinnedShaderProgram, m_pixelShader));
    YDS_NESTED_ERROR_CALL(m_device->LinkProgram(m_skinnedShaderProgram));

    YDS_NESTED_ERROR_CALL(m_device->CreateShaderProgram(&m_consoleProgram));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_consoleProgram, m_consoleVertexShader));
    YDS_NESTED_ERROR_CALL(m_device->AttachShader(m_consoleProgram, m_consolePixelShader));
    YDS_NESTED_ERROR_CALL(m_device->LinkProgram(m_consoleProgram));

    // Create shader controls
    YDS_NESTED_ERROR_CALL(m_device->CreateConstantBuffer(&m_consoleShaderObjectVariablesBuffer,
        sizeof(ConsoleShaderObjectVariables), nullptr));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::InitializeBreakdownTimer(const char *loggingDirectory) {
    YDS_ERROR_DECLARE("InitializeBreakdownTimer");

    ysBreakdownTimerChannel *full = m_breakdownTimer.CreateChannel(FrameBreakdownFull);
    ysBreakdownTimerChannel *scene = m_breakdownTimer.CreateChannel(FrameBreakdownRenderScene);

    std::string logFile = loggingDirectory;
    logFile += "/frame_breakdown_log.csv";

    m_breakdownTimer.OpenLogFile(logFile);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::LoadTexture(ysTexture **image, const char *fname) {
    YDS_ERROR_DECLARE("LoadTexture");

    YDS_NESTED_ERROR_CALL(m_device->CreateTexture(image, fname));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::LoadAnimation(Animation **animation, const char *path, int start, int end) {
    YDS_ERROR_DECLARE("LoadAnimation");

    ysTexture **list = new ysTexture *[end - start + 1];
    char buffer[256];
    for (int i = start; i <= end; ++i) {
        sprintf_s(buffer, 256, "%s/%.4i.png", path, i);

        YDS_NESTED_ERROR_CALL(m_device->CreateTexture(&list[i - start], buffer));
    }

    Animation *newAnimation;
    newAnimation = new Animation;
    newAnimation->m_nFrames = end - start + 1;
    newAnimation->m_textures = list;

    *animation = newAnimation;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::LoadFont(Font **font, const char *path, int size, int fontSize) {
    YDS_ERROR_DECLARE("LoadFont");

    unsigned char *ttfBuffer = new unsigned char[1 << 20];
    unsigned char *bitmapData = new unsigned char[size * size];

    FILE *f = fopen(path, "rb");

    if (f == nullptr) {
        return YDS_ERROR_RETURN(ysError::CouldNotOpenFile);
    }

    fread(ttfBuffer, 1, 1 << 20, f);

    stbtt_packedchar *cdata = new stbtt_packedchar[96];

    int result = 0;
    stbtt_pack_context context;
    result = stbtt_PackBegin(&context, bitmapData, size, size, 0, 16, nullptr);
    result = stbtt_PackFontRange(&context, ttfBuffer, 0, (float)fontSize, 32, 96, cdata);
    stbtt_PackEnd(&context);
    delete[] ttfBuffer;

    Font *newFont = new Font;
    *font = newFont;

    ysTexture *texture = nullptr;
    YDS_NESTED_ERROR_CALL(m_device->CreateAlphaTexture(&texture, size, size, bitmapData));

    newFont->Initialize(32, 96, cdata, (float)fontSize, texture);

    delete[] cdata;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::PlayAudio(AudioAsset *audio) {
    YDS_ERROR_DECLARE("PlayAudio");

    ysAudioSource *newSource = m_audioDevice->CreateSource(audio->GetBuffer());
    newSource->SetMode(ysAudioSource::Mode::PlayOnce);
    newSource->SetPan(0.0f);
    newSource->SetVolume(1.0f);

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::DeltaEngine::SubmitSkeleton(Skeleton *skeleton) {
    const int nBones = skeleton->GetBoneCount();

    for (int i = 0; i < nBones; i++) {
        Bone *bone = skeleton->GetBone(i);
        //m_shaderSkinningControls.BoneTransforms[i] = ysMath::Transpose(skeleton->GetBone(i)->GetSkinMatrix());
    }
}

void dbasic::DeltaEngine::SetWindowSize(int width, int height) {
    /* void */
}

void dbasic::DeltaEngine::SetConsoleColor(const ysVector &v) {
    m_consoleShaderObjectVariables.MulCol = ysMath::GetVector4(v);
}

bool dbasic::DeltaEngine::IsKeyDown(ysKey::Code key) {
    if (m_mainKeyboard != nullptr) {
        return m_mainKeyboard->IsKeyDown(key);
    }

    return false;
}

bool dbasic::DeltaEngine::ProcessKeyDown(ysKey::Code key) {
    if (m_mainKeyboard != nullptr) {
        return m_mainKeyboard->ProcessKeyTransition(key);
    }

    return false;
}

bool dbasic::DeltaEngine::ProcessKeyUp(ysKey::Code key) {
    if (m_mainKeyboard != nullptr) {
        return m_mainKeyboard->ProcessKeyTransition(key, ysKey::State::UpTransition);
    }

    return false;
}

bool dbasic::DeltaEngine::ProcessMouseButtonDown(ysMouse::Button key) {
    if (m_mainMouse != nullptr) {
        return m_mainMouse->ProcessMouseButton(key, ysMouse::ButtonState::DownTransition);
    }

    return false;
}

bool dbasic::DeltaEngine::ProcessMouseButtonUp(ysMouse::Button key) {
    if (m_mainMouse != nullptr) {
        return m_mainMouse->ProcessMouseButton(key, ysMouse::ButtonState::UpTransition);
    }

    return false;
}

bool dbasic::DeltaEngine::IsMouseButtonDown(ysMouse::Button button) {
    if (m_mainMouse != nullptr) {
        return m_mainMouse->IsDown(button);
    }

    return false;
}

int dbasic::DeltaEngine::GetMouseWheel() {
    if (m_mainMouse != nullptr) {
        return m_mainMouse->GetWheel();
    }

    return 0;
}

void dbasic::DeltaEngine::GetMousePos(int *x, int *y) {
    if (m_mainMouse != nullptr) {
        if (x != nullptr) {
            *x = m_mainMouse->GetX();
        }

        if (y != nullptr) {
            *y = m_mainMouse->GetY();
        }
    }
}

void dbasic::DeltaEngine::GetOsMousePos(int *x, int *y) {
    if (m_mainMouse != nullptr) {
        int os_x = m_mainMouse->GetOsPositionX();
        int os_y = m_mainMouse->GetOsPositionY();

        m_gameWindow->ScreenToLocal(os_x, os_y);

        if (x != nullptr) *x = os_x;
        if (y != nullptr) *y = os_y;
    }
}

float dbasic::DeltaEngine::GetFrameLength() {
    return (float)(m_timingSystem->GetFrameDuration());
}

float dbasic::DeltaEngine::GetAverageFramerate() {
    return m_timingSystem->GetFPS();
}

int dbasic::DeltaEngine::GetScreenWidth() const {
    return m_gameWindow->GetGameWidth();
}

int dbasic::DeltaEngine::GetScreenHeight() const {
    return m_gameWindow->GetGameHeight();
}

ysError dbasic::DeltaEngine::DrawSaq(StageEnableFlags flags) {
    YDS_ERROR_DECLARE("DrawSaq");

    DrawCall *newCall = NewDrawCall(0, m_shaderSet->GetObjectDataSize());
    if (newCall != nullptr) {
        YDS_NESTED_ERROR_CALL(m_shaderSet->CacheObjectData(newCall->ObjectData, m_shaderSet->GetObjectDataSize()));
        newCall->Flags = flags;
        newCall->DepthTest = false;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawImage(StageEnableFlags flags, ysTexture *image, int layer) {
    YDS_ERROR_DECLARE("DrawImage");

    DrawCall *newCall = NewDrawCall(layer, m_shaderSet->GetObjectDataSize());
    if (newCall != nullptr) {
        YDS_NESTED_ERROR_CALL(m_shaderSet->CacheObjectData(newCall->ObjectData, m_shaderSet->GetObjectDataSize()));
        newCall->Flags = flags;
        newCall->DepthTest = false;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawBox(StageEnableFlags flags, int layer) {
    YDS_ERROR_DECLARE("DrawBox");

    DrawCall *newCall = NewDrawCall(layer, m_shaderSet->GetObjectDataSize());
    if (newCall != nullptr) {
        YDS_NESTED_ERROR_CALL(m_shaderSet->CacheObjectData(newCall->ObjectData, m_shaderSet->GetObjectDataSize()));
        newCall->Flags = flags;
        newCall->DepthTest = false;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawAxis(StageEnableFlags flags, int layer) {
    YDS_ERROR_DECLARE("DrawAxis");

    YDS_NESTED_ERROR_CALL(DrawBox(flags, layer));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawModel(StageEnableFlags flags, ModelAsset *model, int layer) {
    YDS_ERROR_DECLARE("DrawModel");

    DrawCall *newCall = NewDrawCall(layer, m_shaderSet->GetObjectDataSize());
    if (newCall != nullptr) {
        YDS_NESTED_ERROR_CALL(m_shaderSet->CacheObjectData(newCall->ObjectData, m_shaderSet->GetObjectDataSize()));
        newCall->VertexSize = model->GetVertexSize();
        newCall->IndexBuffer = model->GetIndexBuffer();
        newCall->VertexBuffer = model->GetVertexBuffer();
        newCall->BaseVertex = model->GetBaseVertex();
        newCall->BaseIndex = model->GetBaseIndex();
        newCall->FaceCount = model->GetFaceCount();
        newCall->Flags = flags;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawRenderSkeleton(
    StageEnableFlags flags, RenderSkeleton *skeleton, float scale, ShaderBase *shaders, int layer)
{
    YDS_ERROR_DECLARE("DrawRenderSkeleton");

    const int nodeCount = skeleton->GetNodeCount();
    for (int i = 0; i < nodeCount; ++i) {
        RenderNode *node = skeleton->GetNode(i);
        if (node->GetModelAsset() != nullptr) {
            shaders->SetObjectTransform(node->Transform.GetWorldTransform());
            shaders->ConfigureModel(scale, node->GetModelAsset());
            DrawModel(
                flags,
                node->GetModelAsset(),
                layer);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::DrawGeneric(
    StageEnableFlags flags, ysGPUBuffer *indexBuffer, ysGPUBuffer *vertexBuffer,
    int vertexSize, int baseIndex, int baseVertex, int faceCount, bool depthTest, int layer)
{
    YDS_ERROR_DECLARE("DrawGeneric");

    DrawCall *newCall = NewDrawCall(layer, m_shaderSet->GetObjectDataSize());
    if (newCall != nullptr) {
        YDS_NESTED_ERROR_CALL(m_shaderSet->CacheObjectData(newCall->ObjectData, m_shaderSet->GetObjectDataSize()));
        newCall->VertexSize = vertexSize;
        newCall->IndexBuffer = indexBuffer;
        newCall->VertexBuffer = vertexBuffer;
        newCall->BaseVertex = baseVertex;
        newCall->BaseIndex = baseIndex;
        newCall->FaceCount = faceCount;
        newCall->Flags = flags;
        newCall->DepthTest = depthTest;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::ExecuteDrawQueue() {
    YDS_ERROR_DECLARE("ExecuteDrawQueue");

    const int stageCount = m_shaderSet->GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        ExecuteShaderStage(i);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::DeltaEngine::ExecuteShaderStage(int stageIndex) {
    YDS_ERROR_DECLARE("ExecuteShaderStage");

    ShaderStage *stage = m_shaderSet->GetStage(stageIndex);
    if (stage->GetType() == ShaderStage::Type::FullPass) {
        stage->BindScene();

        for (int i = 0; i < MaxLayers; i++) {
            const int objectsAtLayer = m_drawQueue[i].GetNumObjects();;
            for (int j = 0; j < objectsAtLayer; j++) {
                const DrawCall *call = &m_drawQueue[i][j];
                if (call == nullptr) continue;
                if (!stage->CheckFlags(call->Flags)) continue;

                m_shaderSet->ReadObjectData(call->ObjectData, stageIndex, call->ObjectDataSize);
                stage->BindObject();

                if (call->IndexBuffer != nullptr) {
                    m_device->SetDepthTestEnabled(stage->GetRenderTarget(), call->DepthTest);

                    m_device->UseIndexBuffer(call->IndexBuffer, 0);
                    m_device->UseVertexBuffer(call->VertexBuffer, call->VertexSize, 0);

                    m_device->Draw(call->FaceCount, call->BaseIndex, call->BaseVertex);
                }
                else {
                    m_device->SetDepthTestEnabled(stage->GetRenderTarget(), call->DepthTest);

                    m_device->UseIndexBuffer(m_mainIndexBuffer, 0);
                    m_device->UseVertexBuffer(m_mainVertexBuffer, sizeof(Vertex), 0);

                    m_device->Draw(2, 0, 0);
                }
            }
        }
    }
    else if (stage->GetType() == ShaderStage::Type::PostProcessing) {
        for (int i = 0; i < stage->GetPasses(); ++i) {
            stage->OnPass(i);
            stage->BindScene();
            stage->BindObject();

            m_device->SetDepthTestEnabled(stage->GetRenderTarget(), false);

            m_device->UseIndexBuffer(m_mainIndexBuffer, 0);
            m_device->UseVertexBuffer(m_mainVertexBuffer, sizeof(Vertex), 0);

            m_device->Draw(2, 0, 0);
        }

        stage->OnEnd();
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::DeltaEngine::ClearDrawQueue() {
    for (int i = 0; i < MaxLayers; ++i) {
        const int n = m_drawQueue[i].GetNumObjects();
        for (int j = 0; j < n; ++j) {
            free(m_drawQueue[i][j].ObjectData);
        }
    }

    for (int i = 0; i < MaxLayers; ++i) {
        m_drawQueue[i].Clear();
    }
}
