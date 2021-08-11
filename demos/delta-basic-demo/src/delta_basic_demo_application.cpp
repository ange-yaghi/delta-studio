#include "../include/delta_basic_demo_application.h"

#include <sstream>

delta_demo::DeltaBasicDemoApplication::DeltaBasicDemoApplication() {
    m_demoTexture = nullptr;
}

delta_demo::DeltaBasicDemoApplication::~DeltaBasicDemoApplication() {
    /* void */
}

void delta_demo::DeltaBasicDemoApplication::Initialize(void *instance, ysContextObject::DeviceAPI api) {
    m_engine.GetConsole()->SetDefaultFontDirectory("../../../engines/basic/fonts/");

    dbasic::DeltaEngine::GameEngineSettings settings;
    settings.API = api;
    settings.DepthBuffer = true;
    settings.Instance = instance;
    settings.ShaderDirectory = "../../../engines/basic/shaders/";
    settings.WindowTitle = "Delta Basic Engine - Demo Application";
    settings.WindowPositionX = 0;
    settings.WindowPositionY = 0;
    settings.WindowStyle = ysWindow::WindowStyle::Windowed;

    m_engine.CreateGameWindow(settings); // TODO: path should be relative to exe

    ysWindowSystem *windowSystem = m_engine.GetWindowSystem();
    ysMonitor *monitor = windowSystem->GetPrimaryMonitor();

    ysWindow *window = m_engine.GetGameWindow();
    window->SetWindowSize(monitor->GetPhysicalWidth(), monitor->GetPhysicalHeight());

    dbasic::Material *lightFill = m_assetManager.NewMaterial();
    lightFill->SetName("LightFill");
    lightFill->SetDiffuseColor(ysColor::srgbiToLinear(0xEF, 0x38, 0x37));
    lightFill->SetLit(false);

    dbasic::Material *lineColor = m_assetManager.NewMaterial();
    lineColor->SetName("LineColor");
    lineColor->SetDiffuseColor(ysColor::srgbiToLinear(0x91, 0x1A, 0x1D));
    lineColor->SetLit(false);

    dbasic::Material *darkFill = m_assetManager.NewMaterial();
    darkFill->SetName("DarkFill");
    darkFill->SetDiffuseColor(ysColor::srgbiToLinear(0xC4, 0x21, 0x26));
    darkFill->SetLit(false);

    dbasic::Material *highlight = m_assetManager.NewMaterial();
    highlight->SetName("Highlight");
    highlight->SetDiffuseColor(ysColor::srgbToLinear(1.0f, 1.0f, 1.0f, 1.0f - 0.941667f));
    highlight->SetLit(false);

    m_assetManager.SetEngine(&m_engine);
    m_assetManager.CompileInterchangeFile("../../../test/geometry_files/ant_rigged", 1.0f, true);
    m_assetManager.LoadSceneFile("../../../test/geometry_files/ant_rigged");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject(
        "Armature", ysObjectData::ObjectType::Empty);
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    m_probe = &m_renderSkeleton->FindNode("Head")->Transform;
    m_probe2 = m_renderSkeleton->FindNode("Circle");

    m_assetManager.LoadAnimationFile("../../../test/animation_files/ant_rigged.dimo");
    m_blinkAction = m_assetManager.GetAction("Blink");
    m_idleAction = m_assetManager.GetAction("Idle");
    m_walkAction = m_assetManager.GetAction("Walk");

    m_blinkAction->SetLength(40.0f);
    m_idleAction->SetLength(100.0f);
    m_walkAction->SetLength(30.0f);

    m_renderSkeleton->BindAction(m_blinkAction, &m_blink);
    m_renderSkeleton->BindAction(m_idleAction, &m_idle);
    m_renderSkeleton->BindAction(m_walkAction, &m_walk);

    m_channel1 = m_renderSkeleton->AnimationMixer.NewChannel();
    m_channel2 = m_renderSkeleton->AnimationMixer.NewChannel();

    ysAnimationChannel::ActionSettings paused;
    paused.Speed = 1.0f;
    paused.FadeIn = 5.0f;
    m_channel1->AddSegment(&m_idle, paused);
    m_channel2->AddSegment(&m_blink, paused);

    m_blinkTimer = 4.0f;

    m_engine.InitializeShaderSet(&m_shaderSet);
    m_engine.InitializeDefaultShaders(&m_shaders, &m_shaderSet);
    m_engine.InitializeConsoleShaders(&m_shaderSet);
    m_engine.SetShaderSet(&m_shaderSet);

    m_shaders.SetClearColor(ysColor::srgbiToLinear(0xadd8e6));
}

void delta_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void delta_demo::DeltaBasicDemoApplication::Render() {
    m_shaders.SetCameraPosition(0.0f, 0.0f);
    m_shaders.SetCameraAltitude(20.0f);

    m_renderSkeleton->UpdateAnimation(m_engine.GetFrameLength() * 60);
    m_engine.DrawRenderSkeleton(m_shaders.GetRegularFlags(), m_renderSkeleton, 1.0f, &m_shaders, 0);

    ysAnimationChannel::ActionSettings normalSpeed;
    normalSpeed.Speed = 1.0f;
    normalSpeed.FadeIn = 20.0f;

    ysAnimationChannel::ActionSettings backwardsSpeed;
    backwardsSpeed.Speed = -1.0f;
    backwardsSpeed.FadeIn = 20.0f;

    ysAnimationChannel::ActionSettings fastSpeed;
    fastSpeed.Speed = 2.0f;
    fastSpeed.FadeIn = 10.0f;

    ysAnimationChannel::ActionSettings blinkSpeed;
    blinkSpeed.Speed = 1.0f;
    blinkSpeed.FadeIn = 2.0f;

    if (m_engine.IsKeyDown(ysKey::Code::W)) {
        ysAnimationChannel::ActionSettings *speed = &normalSpeed;
        if (m_engine.IsKeyDown(ysKey::Code::Up)) {
            speed = &fastSpeed;
        }
        else if (m_engine.IsKeyDown(ysKey::Code::Down)) {
            speed = &backwardsSpeed;
        }

        if (m_channel1->GetSpeed() != speed->Speed) {
            m_channel1->ChangeSpeed(speed->Speed);
            m_channel1->ClearQueue();
        }

        if (m_channel1->GetCurrentAction() != &m_walk) {
            m_channel1->AddSegment(&m_walk, *speed);
            m_channel1->ClearQueue();
        }
        else if (!m_channel1->HasQueuedSegments()) {
            speed->FadeIn = 0.0f;
            m_channel1->QueueSegment(&m_walk, *speed);
        }
    }
    else {
        if (m_channel1->GetCurrentAction() != &m_idle) {
            m_channel1->AddSegment(&m_idle, normalSpeed);
            m_channel1->ClearQueue();
        }
        else if (!m_channel1->HasQueuedSegments()) {
            m_channel1->QueueSegment(&m_idle, normalSpeed);
        }
    }

    if (m_engine.ProcessKeyDown(ysKey::Code::B) || m_blinkTimer <= 0) {
        m_blinkTimer = ((rand() % 201 - 100) / 100.0f) * 1.0f + 4.0f;
        m_channel2->AddSegment(&m_blink, blinkSpeed);
    }

    m_blinkTimer -= m_engine.GetFrameLength();

    m_shaders.SetAmbientLight(ysVector4(0.0, 0.0, 0.0, 1.0f));

    dbasic::Console *console = m_engine.GetConsole();
    console->Clear();
    console->MoveToOrigin();

    const int screenWidth = m_engine.GetGameWindow()->GetGameWidth();
    const int screenHeight = m_engine.GetGameWindow()->GetGameHeight();

    m_shaders.SetScreenDimensions((float)screenWidth, (float)screenHeight);
    m_shaders.CalculateCamera();

    if (m_engine.IsKeyDown(ysKey::Code::N1)) {
        m_engine.GetGameWindow()->SetGameResolutionScale(1.0f);
    }
    else if (m_engine.IsKeyDown(ysKey::Code::N2)) {
        m_engine.GetGameWindow()->SetGameResolutionScale(0.5f);
    }
    else if (m_engine.IsKeyDown(ysKey::Code::N3)) {
        m_engine.GetGameWindow()->SetGameResolutionScale(0.2222f);
    }

    if (m_engine.ProcessKeyDown(ysKey::Code::F)) {
        switch (m_engine.GetGameWindow()->GetWindowStyle()) {
        case ysWindow::WindowStyle::Windowed:
            m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Popup);
            break;
        case ysWindow::WindowStyle::Popup:
            m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
            break;
        case ysWindow::WindowStyle::Fullscreen:
            m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Windowed);
            break;
        }
    }

    std::stringstream msg;
    msg << "FPS " << m_engine.GetAverageFramerate() << "\n";
    msg << screenWidth << "x" << screenHeight << "\n";
    console->DrawGeneralText(msg.str().c_str());
}

void delta_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        if (m_engine.IsKeyDown(ysKey::Code::Escape)) {
            break;
        }

        Process();
        Render();

        m_engine.EndFrame();
    }
}

void delta_demo::DeltaBasicDemoApplication::Destroy() {
    m_shaderSet.Destroy();
    m_shaders.Destroy();
    m_assetManager.Destroy();
    m_engine.Destroy();
}
