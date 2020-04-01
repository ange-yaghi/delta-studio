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

    m_assetManager.CompileInterchangeFile("../../workspace/armature_test", 1.0f, true);
    m_assetManager.LoadSceneFile("../../workspace/armature_test");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject("Armature");
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(0.0f, 0.0f, 0.0f));

    m_bone001 = &m_renderSkeleton->FindNode("Bone.001")->RigidBody;
    m_bone = &m_renderSkeleton->FindNode("Bone")->RigidBody;
    
    m_engine.LoadTexture(&m_demoTexture, "../../demos/delta-basic-demo/assets/chicken.png");

    ysAnimationInterchangeFile0_0 interchangeFile;
    interchangeFile.Open("../../test/animation_files/armature_test.dimo");
    interchangeFile.ReadAction(&m_riseAction);
    interchangeFile.ReadAction(&m_twistAction);
    interchangeFile.Close();

    m_riseBinding.SetAction(&m_riseAction);
    m_twistBinding.SetAction(&m_twistAction);

    m_riseBinding.AddTarget("Bone", nullptr, &m_boneTarget);
    m_twistBinding.AddTarget("Bone.001", nullptr, &m_bone001Target);

    m_boneBase = m_bone001->GetOrientation();
    m_bone001Base = m_bone001->GetOrientation();

    m_currentAngle = 0.0f;

    m_channel1 = m_animationMixer.NewChannel();
    m_channel2 = m_animationMixer.NewChannel();

    m_boneTarget.ClearRotation(ysMath::Constants::QuatIdentity);
    m_bone001Target.ClearRotation(ysMath::Constants::QuatIdentity);
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

    m_boneTarget.ClearFlags();
    m_bone001Target.ClearFlags();

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_A)) {
        m_channel1->AddSegment(&m_riseBinding, 5.0f);
    }

    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_S)) {
        m_channel2->AddSegment(&m_twistBinding, 5.0f);
    }

    m_animationMixer.Sample();
    m_animationMixer.Advance(m_engine.GetFrameLength() * 5);

    m_bone->SetOrientation(ysMath::QuatMultiply(m_boneTarget.GetQuaternionResult(), m_boneBase));
    m_bone001->SetOrientation(ysMath::QuatMultiply(m_bone001Target.GetQuaternionResult(), m_bone001Base));
}

void dbasic_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }
}
