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

    m_assetManager.CompileInterchangeFile("../../test/geometry_files/armature_test", 1.0f, true);
    m_assetManager.LoadSceneFile("../../test/geometry_files/armature_test");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject("Armature");
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));
    
    m_engine.LoadTexture(&m_demoTexture, "../../demos/delta-basic-demo/assets/chicken.png");

    m_assetManager.LoadAnimationFile("../../test/animation_files/armature_test.dimo");
    m_riseAction = m_assetManager.GetAction("Rise");
    m_twistAction = m_assetManager.GetAction("Twist");

    m_riseAction->SetLength(40.0f);
    m_twistAction->SetLength(40.0f);

    m_renderSkeleton->BindAction(m_riseAction, &m_riseBinding);
    m_renderSkeleton->BindAction(m_twistAction, &m_twistBinding);

    m_currentAngle = 0.0f;

    m_channel1 = m_renderSkeleton->AnimationMixer.NewChannel();
    m_channel2 = m_renderSkeleton->AnimationMixer.NewChannel();

    ysAnimationChannel::ActionSettings paused;
    paused.Speed = 0.0f;
    paused.FadeIn = 0.0f;
    m_channel1->AddSegment(&m_riseBinding, paused);
    m_channel2->AddSegment(&m_twistBinding, paused);
}

void dbasic_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Render() {
    m_engine.SetCameraPosition(0.0f, 0.0f);
    m_engine.SetCameraAltitude(15.0f);

    m_currentAngle += 0.5f;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;

    m_engine.SetMultiplyColor(ysVector4(0xe7 / 255.0f, 0x4c / 255.0f, 0x3c / 255.0f, 1.0f));

    ysQuaternion q = ysMath::Constants::QuatIdentity;
    //q = ysMath::LoadQuaternion(m_currentAngle * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    q = ysMath::Constants::QuatIdentity;
    //m_skeletonBase.SetOrientation(q);

    q = ysMath::LoadQuaternion(m_currentAngle * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(0.0f, 0.0f, 1.0f));
    //m_rod->SetOrientation(q);

    m_skeletonBase.UpdateDerivedData(true);
    srand(0);
    for (int i = 0; i < m_renderSkeleton->GetNodeCount(); ++i) {
        dbasic::ModelAsset *asset = m_renderSkeleton->GetNode(i)->GetModelAsset();
        m_engine.SetMultiplyColor(ysVector4((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, 1.0f));
        if (asset != nullptr) m_engine.DrawModel(asset, m_renderSkeleton->GetNode(i)->RigidBody.GetTransform(), 1.0f, nullptr);
    }

    ysAnimationChannel::ActionSettings doubleSpeed, normalSpeed;
    doubleSpeed.Speed = 1.0f;
    doubleSpeed.FadeIn = 5.0f;

    normalSpeed.Speed = 0.5f;
    normalSpeed.FadeIn = 5.0f;

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_A)) {
        if (m_engine.IsKeyDown(ysKeyboard::KEY_UP)) {
            m_channel1->AddSegment(&m_riseBinding, doubleSpeed);
        }
        else {
            m_channel1->AddSegment(&m_riseBinding, normalSpeed);
        }
    }

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_S)) {
        if (m_engine.IsKeyDown(ysKeyboard::KEY_UP)) {
            m_channel2->AddSegment(&m_twistBinding, doubleSpeed);
        }
        else {
            m_channel2->AddSegment(&m_twistBinding, normalSpeed);
        }
    }

    m_renderSkeleton->UpdateAnimation(m_engine.GetFrameLength() * 60);
}

void dbasic_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }
}
