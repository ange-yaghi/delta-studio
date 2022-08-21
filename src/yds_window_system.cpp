#include "../include/yds_window_system.h"

#include "../include/yds_input_system.h"
#if PLATFORM_WIN32
#include "../include/yds_windows_window_system.h"
#endif
#if PLATFORM_SDL
#include "../include/yds_sdl_window_system.h"
#endif

ysWindowSystem *ysWindowSystem::g_instance = nullptr;

ysWindowSystem::ysWindowSystem() : ysWindowSystemObject("WINDOW_SYSTEM", Platform::Unknown) {
    YDS_ERROR_DECLARE("ysWindowSystem");

    if (g_instance != nullptr) YDS_ERROR_RETURN(ysError::MultipleSystems);
    g_instance = this;

    m_inputSystem = nullptr;
    m_cursorVisible = true;
    m_cursorConfined = false;

    YDS_ERROR_RETURN(ysError::None);
}

ysWindowSystem::ysWindowSystem(Platform platform) : ysWindowSystemObject("WINDOW_SYSTEM", platform) {
    YDS_ERROR_DECLARE("ysWindowSystem");

    if (g_instance != nullptr) YDS_ERROR_RETURN(ysError::MultipleSystems);
    g_instance = this;

    m_cursorVisible = true;
    m_cursorConfined = false;
    m_inputSystem = nullptr;

    YDS_ERROR_RETURN(ysError::None);
}

ysWindowSystem::~ysWindowSystem() {
    /* void */
}

ysError ysWindowSystem::CreateWindowSystem(ysWindowSystem **newSystem, Platform platform) {
    YDS_ERROR_DECLARE("CreateWindowSystem");

    if (newSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    *newSystem = nullptr;

    if (platform == Platform::Unknown) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    if (g_instance != nullptr) return YDS_ERROR_RETURN_STATIC(ysError::MultipleErrorSystems);

    switch (platform) {
    case Platform::Windows:
#if PLATFORM_WIN32
        *newSystem = new ysWindowsWindowSystem();
#endif
        break;
    case Platform::Sdl:
#if PLATFORM_SDL
        *newSystem = new ysSdlWindowSystem();
#endif
        break;
    default:
        break;
    }

    if (*newSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::NoPlatform);

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysWindowSystem::DestroyWindowSystem(ysWindowSystem *&system) {
    YDS_ERROR_DECLARE("DestroyWindowSystem");

    delete system;
    system = nullptr;

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysWindowSystem *ysWindowSystem::Get() {
    return g_instance;
}

int ysWindowSystem::GetWindowCount() const {
    return m_windowArray.GetNumObjects();
}

ysWindow *ysWindowSystem::GetWindow(int index) {
    return m_windowArray.Get(index);
}

void ysWindowSystem::CloseWindow(ysWindow *window) {
    window->Close();
}

void ysWindowSystem::DeleteWindow(ysWindow *window) {
    m_windowArray.Delete(window->GetIndex());
}

void ysWindowSystem::CloseAllWindows() {
    const int nWindows = m_windowArray.GetNumObjects();
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

    if (!CheckCompatibility(system)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    system->AssignWindowSystem(this);
    m_inputSystem = system;

    return YDS_ERROR_RETURN(ysError::None);
}
