#include "../include/delta_basic_demo_application.h"

dbasic_demo::DeltaBasicDemoApplication::DeltaBasicDemoApplication() {
    m_currentAngle = 0.0f;
    m_demoTexture = nullptr;
}

dbasic_demo::DeltaBasicDemoApplication::~DeltaBasicDemoApplication() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Initialize(void *instance, ysContextObject::DEVICE_API api) {
    m_engine.GetConsole()->SetDefaultFontDirectory("../../engines/basic/fonts/");

    m_engine.CreateGameWindow(
        "Delta Basic Engine - Demo Application", 
        instance,
        api,
        "../../engines/basic/shaders/"); // TODO: path should be relative to exe
    m_engine.SetClearColor(0x34, 0x98, 0xdb);

    dbasic::Material *lightFill = m_assetManager.NewMaterial();
    lightFill->SetName("LightFill");
    lightFill->SetDiffuseColor(ysVector4(0xEF / 255.0f, 0x38 / 255.0f, 0x37 / 255.0f, 1.0f));

    dbasic::Material *lineColor = m_assetManager.NewMaterial();
    lineColor->SetName("LineColor");
    lineColor->SetDiffuseColor(ysVector4(0x91 / 255.0f, 0x1A / 255.0f, 0x1D / 255.0f, 1.0f));

    dbasic::Material *darkFill = m_assetManager.NewMaterial();
    darkFill->SetName("DarkFill");
    darkFill->SetDiffuseColor(ysVector4(0xC4 / 255.0f, 0x21 / 255.0f, 0x26 / 255.0f, 1.0f));

    dbasic::Material *highlight = m_assetManager.NewMaterial();
    highlight->SetName("Highlight");
    highlight->SetDiffuseColor(ysVector4(1.0f, 1.0f, 1.0f, 1.0f - 0.941667f));

    m_assetManager.SetEngine(&m_engine);
    m_assetManager.CompileInterchangeFile("../../test/geometry_files/ant_rigged", 1.0f, true);
    m_assetManager.LoadSceneFile("../../test/geometry_files/ant_rigged");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject("Armature");
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    m_probe = &m_renderSkeleton->FindNode("Head")->Transform;
    m_probe2 = m_renderSkeleton->FindNode("Circle");

    m_assetManager.LoadAnimationFile("../../test/animation_files/ant_rigged.dimo");
    m_blinkAction = m_assetManager.GetAction("Blink");
    m_idleAction = m_assetManager.GetAction("Idle");
    m_walkAction = m_assetManager.GetAction("Walk");

    m_blinkAction->SetLength(40.0f);
    m_idleAction->SetLength(100.0f);
    m_walkAction->SetLength(30.0f);

    m_renderSkeleton->BindAction(m_blinkAction, &m_blink);
    m_renderSkeleton->BindAction(m_idleAction, &m_idle);
    m_renderSkeleton->BindAction(m_walkAction, &m_walk);

    m_currentAngle = 0.0f;

    m_channel1 = m_renderSkeleton->AnimationMixer.NewChannel();
    m_channel2 = m_renderSkeleton->AnimationMixer.NewChannel();

    ysAnimationChannel::ActionSettings paused;
    paused.Speed = 1.0f;
    paused.FadeIn = 5.0f;
    m_channel1->AddSegment(&m_idle, paused);
    m_channel2->AddSegment(&m_blink, paused);

    m_blinkTimer = 4.0f;
}

void dbasic_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Render() {
    m_engine.SetCameraPosition(0.0f, 0.0f);
    m_engine.SetCameraAltitude(20.0f);

    m_currentAngle += 1.0f;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;

    m_engine.SetMultiplyColor(ysVector4(0xe7 / 255.0f, 0x4c / 255.0f, 0x3c / 255.0f, 1.0f));

    ysQuaternion q = ysMath::Constants::QuatIdentity;
    q = ysMath::LoadQuaternion(45 * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    //m_skeletonBase.SetOrientation(q);

    q = ysMath::LoadQuaternion(m_currentAngle * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(0.0f, 0.0f, 1.0f));
    //m_skeletonBase.SetOrientation(q);

    m_renderSkeleton->UpdateAnimation(m_engine.GetFrameLength() * 60);
    //m_renderSkeleton->GetNode()

    //m_skeletonBase.UpdateDerivedData(true);
    int color[] = { 0xff, 0x0, 0x0 };
    //m_engine.SetObjectTransform(ysMath::LoadMatrix(q));
    //m_engine.DrawBox(color, 5.0f, 5.0f, 0);
    m_engine.DrawRenderSkeleton(m_renderSkeleton, 1.0f, 0);

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

    if (m_engine.IsKeyDown(ysKeyboard::KEY_W)) {
        ysAnimationChannel::ActionSettings *speed = &normalSpeed;
        if (m_engine.IsKeyDown(ysKeyboard::KEY_UP)) {
            speed = &fastSpeed;
        }
        else if (m_engine.IsKeyDown(ysKeyboard::KEY_DOWN)) {
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

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_B) || m_blinkTimer <= 0) {
        m_blinkTimer = ((rand() % 201 - 100) / 100.0f) * 1.0f + 4.0f;
        m_channel2->AddSegment(&m_blink, blinkSpeed);
    }

    m_blinkTimer -= m_engine.GetFrameLength();

    m_engine.SetAmbientLight(ysVector4(0.5, 0.5, 0.5, 1.0f));

    dbasic::Light light;
    light.Position = ysVector4(0.0f, 0.0f, 2.0f, 0.0f);
    light.Color = ysVector4(1.0f, 1.0f, 1.0f, 0.0f);
    light.Direction = ysMath::GetVector4(ysMath::Normalize(ysMath::LoadVector(1.0f, 0.0f, -1.0f)));
    light.Attenuation0 = 0.9f;
    light.Attenuation1 = 0.89f;
    m_engine.AddLight(light);
}

void dbasic_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }

    m_assetManager.Destroy();
    m_engine.Destroy();
}
