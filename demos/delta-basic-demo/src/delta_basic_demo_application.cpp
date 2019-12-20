#include "..\include\delta_basic_demo_application.h"

dbasic_demo::DeltaBasicDemoApplication::DeltaBasicDemoApplication() {
    /* void */
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
    m_engine.SetClearColor(121, 149, 164);

    m_assetManager.SetEngine(&m_engine);
    m_assetManager.CompileSceneFile("../../workspace/addon_dev", 1.0f, true);
    m_assetManager.LoadSceneFile("../../workspace/addon_dev");

    m_currentAngle = 0.0f;
}

void dbasic_demo::DeltaBasicDemoApplication::Process() {
    /* void */
}

void dbasic_demo::DeltaBasicDemoApplication::Render() {
    m_engine.SetCameraPosition(0.0f, 0.0f);
    m_engine.SetCameraAltitude(10.0f);

    m_currentAngle += 0.1f;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;

    m_engine.DrawModel(m_assetManager.GetModelAsset(0), ysMath::RotationTransform(ysMath::Constants::XAxis, m_currentAngle * ysMath::Constants::PI / 180.0f), 1.0f, nullptr);

    int color[] = { 255, 255, 255 };
    //m_engine.DrawBox(color, 15.0f, 2.5f);
}

void dbasic_demo::DeltaBasicDemoApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }
}
