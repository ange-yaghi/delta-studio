#include "../include/empty_window_demo.h"

delta_demo::EmptyWindowDemo::EmptyWindowDemo() {
    m_window = nullptr;
    m_windowSystem = nullptr;
    m_device = nullptr;
    m_renderTarget = nullptr;
}

delta_demo::EmptyWindowDemo::~EmptyWindowDemo() {
    /* void */
}

void delta_demo::EmptyWindowDemo::Initialize(void *instance, ysContextObject::DeviceAPI api) {
    ysError result;

    result =
        ysWindowSystem::CreateWindowSystem(&m_windowSystem, ysWindowSystemObject::Platform::Windows);
    if (result != ysError::None) return;

    m_windowSystem->ConnectInstance(instance);

    result = m_windowSystem->NewWindow(&m_window);
    if (result != ysError::None) return;

    m_windowSystem->SurveyMonitors();
    ysMonitor *mainMonitor = m_windowSystem->GetMonitor(0);

    result = m_window->InitializeWindow(
        nullptr,
        "01-Empty Window Demo",
        ysWindow::WindowStyle::Windowed,
        mainMonitor);
    if (result != ysError::None) return;

    result = ysDevice::CreateDevice(&m_device, api);
    if (result != ysError::None) return;

    result = m_device->InitializeDevice();
    if (result != ysError::None) return;

    result = m_device->CreateRenderingContext(&m_context, m_window);
    if (result != ysError::None) return;

    result = m_device->CreateOnScreenRenderTarget(&m_renderTarget, m_context, false);
    if (result != ysError::None) return;

    m_device->SetRenderTarget(m_renderTarget);
}

void delta_demo::EmptyWindowDemo::Run() {
    while (m_window->IsOpen()) {
        m_windowSystem->ProcessMessages();

        if (!m_window->IsOpen()) {
            break;
        }

        const float clearColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        m_device->ClearBuffers(clearColor);

        m_device->Present();
    }
}

void delta_demo::EmptyWindowDemo::Destroy() {
    m_device->DestroyRenderingContext(m_context);
    m_device->DestroyRenderTarget(m_renderTarget);
    m_device->DestroyDevice();

    m_windowSystem->DeleteWindow(m_window);
    ysWindowSystem::DestroyWindowSystem(m_windowSystem);
}

void delta_demo::EmptyWindowDemo::Process() {
    /* void */
}

void delta_demo::EmptyWindowDemo::Render() {
    /* void */
}
