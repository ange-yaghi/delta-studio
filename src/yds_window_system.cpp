#include "../include/yds_window_system.h"

#include "../include/yds_input_system.h"
#include "../include/yds_windows_window_system.h"

ysWindowSystem *ysWindowSystem::g_instance = NULL;

ysWindowSystem::ysWindowSystem() : ysWindowSystemObject("WINDOW_SYSTEM", Platform::UNKNOWN) {
    YDS_ERROR_DECLARE("ysWindowSystem");

    if (g_instance) YDS_ERROR_RETURN(ysError::YDS_MULTIPLE_SYSTEMS);
    g_instance = this;

    m_inputSystem = nullptr;

    YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysWindowSystem::ysWindowSystem(Platform platform) : ysWindowSystemObject("WINDOW_SYSTEM", platform) {
    YDS_ERROR_DECLARE("ysWindowSystem");

    if (g_instance) YDS_ERROR_RETURN(ysError::YDS_MULTIPLE_SYSTEMS);
    g_instance = this;

    m_inputSystem = nullptr;

    YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysWindowSystem::~ysWindowSystem() {
    /* void */
}

ysError ysWindowSystem::CreateWindowSystem(ysWindowSystem **newSystem, Platform platform) {
    YDS_ERROR_DECLARE("CreateWindowSystem");

    if (newSystem == NULL) return YDS_ERROR_RETURN_STATIC(ysError::YDS_INVALID_PARAMETER);
    *newSystem = NULL;

    if (platform == Platform::UNKNOWN) return YDS_ERROR_RETURN_STATIC(ysError::YDS_INVALID_PARAMETER);
    if (g_instance) return YDS_ERROR_RETURN_STATIC(ysError::YDS_MULTIPLE_ERROR_SYSTEMS);

    switch (platform) {
    case Platform::WINDOWS:
        *newSystem = new ysWindowsWindowSystem();
        break;
    default:
        break;
    }

    return YDS_ERROR_RETURN_STATIC(ysError::YDS_NO_ERROR);
}

ysWindowSystem *ysWindowSystem::Get() {
    return g_instance;
}

void ysWindowSystem::CloseWindow(ysWindow *window) {
    window->Close();
}

void ysWindowSystem::DeleteWindow(ysWindow *window) {
    m_windowArray.Delete(window->GetIndex());
}

void ysWindowSystem::CloseAllWindows() {
    int nWindows = m_windowArray.GetNumObjects();
    for (int i = 0; i < nWindows; i++) {
        m_windowArray.Get(i)->Close();
    }
}

void ysWindowSystem::DeleteInactiveWindows() {
    int nWindows = m_windowArray.GetNumObjects();
    for (int i = 0; i < nWindows; i++) {
        if (!m_windowArray.Get(i)->IsOpen()) {
            DeleteWindow(m_windowArray.Get(i));

            if (--i < 0) i = 0;
            nWindows--;
        }
    }
}

void ysWindowSystem::DeleteAllWindows() {
    int nWindows = m_windowArray.GetNumObjects();
    for (int i = 0; i < nWindows; i++) {
        if (m_windowArray.Get(i)->IsOpen()) m_windowArray.Get(i)->Close();
        else {
            DeleteWindow(m_windowArray.Get(i));

            if (--i < 0) i = 0;
            nWindows--;
        }
    }
}

int ysWindowSystem::GetNumMonitors() {
    return m_monitorArray.GetNumObjects();
}

void ysWindowSystem::ClearMonitors() {
    m_monitorArray.Clear();
}

ysMonitor *ysWindowSystem::GetPrimaryMonitor() {
    return m_monitorArray.Get(0);
}

ysMonitor *ysWindowSystem::GetMonitor(int index) {
    return m_monitorArray.Get(index);
}

void ysWindowSystem::SurveyMonitors() {
    ClearMonitors();
}

ysError ysWindowSystem::AssignInputSystem(ysInputSystem *system) {
    YDS_ERROR_DECLARE("AssignInputSystem");

    if (!CheckCompatibility(system)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    system->AssignWindowSystem(this);
    m_inputSystem = system;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
