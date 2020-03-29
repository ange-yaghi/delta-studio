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

    m_assetManager.CompileInterchangeFile("../../workspace/character", 1.0f, true);
    m_assetManager.LoadSceneFile("../../workspace/character");

    m_assetManager.ResolveNodeHierarchy();

    dbasic::SceneObjectAsset *root = m_assetManager.GetSceneObject("Instance_1");
    m_renderSkeleton = m_assetManager.BuildRenderSkeleton(&m_skeletonBase, root);
    m_skeletonBase.SetPosition(ysMath::LoadVector(5.0f, 0.0f, 0.0f));
    
    m_engine.LoadTexture(&m_demoTexture, "../../demos/delta-basic-demo/assets/chicken.png");

    m_currentAngle = 0.0f;
}

void dbasic_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Render() {
    m_engine.SetCameraPosition(0.0f, 0.0f);
    m_engine.SetCameraAltitude(10.0f);

    m_currentAngle += 0.5f;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;

    m_engine.SetMultiplyColor(ysVector4(0xe7 / 255.0f, 0x4c / 255.0f, 0x3c / 255.0f, 1.0f));
    ysMatrix rotation = ysMath::RotationTransform(ysMath::Constants::XAxis, m_currentAngle * ysMath::Constants::PI / 180.0f);
    ysMatrix translation = ysMath::TranslationTransform(ysMath::LoadVector(3.0f, 0.0f, 0.0f));
    m_engine.DrawModel(m_assetManager.GetModelAsset(0), ysMath::MatMult(rotation, translation), 1.0f, nullptr);

    int color[] = { 0xf1, 0xc4, 0x0f };
    m_engine.SetObjectTransform(ysMath::LoadIdentity());
    m_engine.DrawBox(color, 2.5f, 2.5f);

    m_engine.SetMultiplyColor(ysVector4(1.0f, 1.0f, 1.0f, 1.0f));
    translation = ysMath::TranslationTransform(ysMath::LoadVector(-3.0f, 0.0f, 0.0f));
    m_engine.SetObjectTransform(translation);
    m_engine.DrawImage(m_demoTexture, 0, (float)m_demoTexture->GetWidth() / m_demoTexture->GetHeight());

    ysQuaternion q = ysMath::Constants::QuatIdentity;
    q = ysMath::LoadQuaternion(m_currentAngle * ysMath::Constants::PI / 180.0f, ysMath::LoadVector(1.0f, 0.0f, 0.0f));
    m_skeletonBase.SetOrientation(q);

    m_skeletonBase.UpdateDerivedData();
    for (int i = 0; i < m_renderSkeleton->GetNodeCount(); ++i) {
        dbasic::ModelAsset *asset = m_renderSkeleton->GetNode(i)->GetModelAsset();
        if (asset != nullptr) m_engine.DrawModel(asset, m_renderSkeleton->GetNode(i)->RigidBody.GetTransform(), 1.0f, nullptr);
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
