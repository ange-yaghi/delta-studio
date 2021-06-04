#include "../include/multiple_window_demo.h"

delta_demo::MultipleWindowDemo::MultipleWindowDemo() {
    m_window1 = nullptr;
    m_window2 = nullptr;

    m_windowSystem = nullptr;
    m_device = nullptr;

    m_renderTarget1 = nullptr;
    m_renderTarget2 = nullptr;

    m_context1 = nullptr;
    m_context2 = nullptr;
}

delta_demo::MultipleWindowDemo::~MultipleWindowDemo() {
    /* void */
}

void delta_demo::MultipleWindowDemo::Initialize(void *instance, ysContextObject::DeviceAPI api) {
    ysError result;

    result =
        ysWindowSystem::CreateWindowSystem(&m_windowSystem, ysWindowSystemObject::Platform::Windows);
    if (result != ysError::None) return;

    m_windowSystem->ConnectInstance(instance);

    result = m_windowSystem->NewWindow(&m_window1);
    if (result != ysError::None) return;

    result = m_windowSystem->NewWindow(&m_window2);
    if (result != ysError::None) return;

    m_windowSystem->SurveyMonitors();
    ysMonitor *mainMonitor = m_windowSystem->GetMonitor(0);

    result = m_window1->InitializeWindow(
        nullptr,
        "01-Multiple Window Demo Window 1",
        ysWindow::WindowStyle::Windowed,
        mainMonitor);
    if (result != ysError::None) return;

    result = m_window2->InitializeWindow(
        nullptr,
        "01-Multiple Window Demo Window 2",
        ysWindow::WindowStyle::Windowed,
        mainMonitor);
    if (result != ysError::None) return;

    result = ysDevice::CreateDevice(&m_device, api);
    if (result != ysError::None) return;

    result = m_device->InitializeDevice();
    if (result != ysError::None) return;

    result = m_device->CreateRenderingContext(&m_context1, m_window1);
    if (result != ysError::None) return;

    result = m_device->CreateRenderingContext(&m_context2, m_window2);
    if (result != ysError::None) return;

    result = m_device->CreateOnScreenRenderTarget(&m_renderTarget1, m_context1, false);
    if (result != ysError::None) return;

    result = m_device->CreateOnScreenRenderTarget(&m_renderTarget2, m_context2, false);
    if (result != ysError::None) return;
}

void delta_demo::MultipleWindowDemo::Run() {
    while (m_window1->IsOpen() && m_window2->IsOpen()) {
        m_windowSystem->ProcessMessages();

        if (!m_window1->IsOpen() || !m_window2->IsOpen()) {
            break;
        }
        
        m_device->SetRenderTarget(m_renderTarget1);
        const float clearColor0[] = { 0.0f, 0.0f, 1.0f, 1.0f };
        m_device->ClearBuffers(clearColor0);

        m_device->Present();

        m_device->SetRenderTarget(m_renderTarget2);
        const float clearColor1[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        m_device->ClearBuffers(clearColor1);

        m_device->Present();
    }
}

void delta_demo::MultipleWindowDemo::Destroy() {
    m_device->DestroyRenderingContext(m_context1);
    m_device->DestroyRenderingContext(m_context2);

    m_device->DestroyRenderTarget(m_renderTarget1);
    m_device->DestroyRenderTarget(m_renderTarget2);

    m_device->DestroyDevice();

    m_windowSystem->DeleteWindow(m_window1);
    m_windowSystem->DeleteWindow(m_window2);

    ysWindowSystem::DestroyWindowSystem(m_windowSystem);
}

void delta_demo::MultipleWindowDemo::Process() {
    /* void */
}

void delta_demo::MultipleWindowDemo::Render() {
    /* void */
}
