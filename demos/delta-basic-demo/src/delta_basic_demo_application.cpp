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

    m_assetManager.SetEngine(&m_engine);
    m_assetManager.CompileSceneFile("../../workspace/addon_dev", 1.0f, true);
    m_assetManager.LoadSceneFile("../../workspace/addon_dev");

    m_assetManager.CompileInterchangeFile("../../workspace/ant_rigged", 1.0f, true);
    m_assetManager.LoadSceneFile("../../workspace/ant_rigged");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject("Armature");
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    m_probe = &m_renderSkeleton->FindNode("Head")->RigidBody;
    m_probe2 = m_renderSkeleton->FindNode("Circle");

    auto test = m_assetManager.GetSceneObject("Circle");
    
    m_engine.LoadTexture(&m_demoTexture, "../../demos/delta-basic-demo/assets/chicken.png");

    m_assetManager.LoadAnimationFile("../../workspace/ant_rigged.dimo");
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
}

void dbasic_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Render() {
    m_engine.SetCameraPosition(0.0f, 0.0f);
    m_engine.SetCameraAltitude(20.0f);

    m_currentAngle += 0.5f;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;

    m_engine.SetMultiplyColor(ysVector4(0xe7 / 255.0f, 0x4c / 255.0f, 0x3c / 255.0f, 1.0f));

    ysQuaternion q = ysMath::Constants::QuatIdentity;
    q = ysMath::LoadQuaternion(45 * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    //m_skeletonBase.SetOrientation(q);

    q = ysMath::LoadQuaternion(m_currentAngle * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(0.0f, 0.0f, 1.0f));

    m_renderSkeleton->UpdateAnimation(m_engine.GetFrameLength() * 60);

    m_skeletonBase.UpdateDerivedData(true);
    srand(0);
    for (int i = 0; i < m_renderSkeleton->GetNodeCount(); ++i) {
        dbasic::ModelAsset *asset = m_renderSkeleton->GetNode(i)->GetModelAsset();
        m_engine.SetMultiplyColor(ysVector4((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, 1.0f));
        if (asset != nullptr) m_engine.DrawModel(asset, m_renderSkeleton->GetNode(i)->RigidBody.GetTransform(), 1.0f, nullptr);
    }

    ysAnimationChannel::ActionSettings normalSpeed;
    normalSpeed.Speed = 1.0f;
    normalSpeed.FadeIn = 20.0f;

    ysAnimationChannel::ActionSettings blinkSpeed;
    blinkSpeed.Speed = 1.0f;
    blinkSpeed.FadeIn = 2.0f;

    if (m_engine.IsKeyDown(ysKeyboard::KEY_W)) {
        if (m_channel1->GetCurrentAction() != &m_walk) {
            m_channel1->AddSegment(&m_walk, normalSpeed);
        }
        else if (m_channel1->GetPlayhead() > 20.0f) {
            m_channel1->AddSegmentAtEnd(&m_walk, normalSpeed);
        }
    }
    else {
        if (m_channel1->GetCurrentAction() != &m_idle) {
            m_channel1->AddSegment(&m_idle, normalSpeed);
        }
        else if (m_channel1->GetPlayhead() > 50.0f) {
            m_channel1->AddSegmentAtEnd(&m_idle, normalSpeed);
        }
    }

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_B)) {
        m_channel2->AddSegment(&m_blink, blinkSpeed);
    }
}

void dbasic_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }
}
