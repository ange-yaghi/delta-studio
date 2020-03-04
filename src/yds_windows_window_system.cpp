#include "../include/yds_windows_window_system.h"

#include "../include/yds_windows_window.h"
#include "../include/yds_windows_monitor.h"
#include "../include/yds_windows_input_system.h"

ysWindowsWindowSystem::ysWindowsWindowSystem() : ysWindowSystem(Platform::WINDOWS) {
    m_instance = NULL;
}

ysWindowsWindowSystem::~ysWindowsWindowSystem() {
    /* void */
}

ysError ysWindowsWindowSystem::NewWindow(ysWindow **newWindow) {
    YDS_ERROR_DECLARE("NewWindow");

    if (newWindow == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    if (m_instance == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_CONTEXT);

    ysWindowsWindow *windowsWindow = m_windowArray.NewGeneric<ysWindowsWindow>();
    windowsWindow->SetInstance(m_instance);

    *newWindow = static_cast<ysWindow *>(windowsWindow);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysMonitor *ysWindowsWindowSystem::NewMonitor() {
    return m_monitorArray.NewGeneric<ysWindowsMonitor>();
}

BOOL CALLBACK MonitorEnum(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    ysWindowSystem *windowSystem = (ysWindowSystem *)dwData;
    ysMonitor *newMonitor = windowSystem->NewMonitor();

    MONITORINFOEX info;
    info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &info);

    newMonitor->SetOrigin(lprcMonitor->left, lprcMonitor->top);
    newMonitor->SetSize(lprcMonitor->right - lprcMonitor->left,
        lprcMonitor->bottom - lprcMonitor->top);
    newMonitor->SetDeviceName(info.szDevice);

    return TRUE;
}

void ysWindowsWindowSystem::SurveyMonitors() {
    ysWindowSystem::SurveyMonitors();

    EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)this);
}

void ysWindowsWindowSystem::ProcessMessages() {
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

ysWindow *ysWindowsWindowSystem::FindWindowFromHandle(HWND handle) {
    for (int i = 0; i < m_windowArray.GetNumObjects(); i++) {
        ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(m_windowArray.Get(i));

        if (windowsWindow->GetWindowHandle() == handle) return windowsWindow;
    }

    return NULL;
}

void ysWindowsWindowSystem::ConnectInstance(void *genericInstanceConnection) {
    HINSTANCE *instance = static_cast<HINSTANCE *>(genericInstanceConnection);

    m_instance = *instance;
}

LRESULT WINAPI ysWindowsWindowSystem::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int id = GetCurrentThreadId();

    ysWindowSystem *system = ysWindowSystem::Get();

    ysWindowsWindowSystem *windowsSystem = static_cast<ysWindowsWindowSystem *>(system);
    ysWindowsInputSystem *inputSystem = static_cast<ysWindowsInputSystem *>(windowsSystem->GetInputSystem());

    ysWindow *target = windowsSystem->FindWindowFromHandle(hWnd);
    if (target) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            target->OnCloseWindow();
            ysWindowSystem::Get()->CloseWindow(target);
            return 0;
        case WM_SIZE:
            target->OnResizeWindow(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_MOVE:
            target->OnMoveWindow(LOWORD(lParam), HIWORD(lParam));
            return 0;

            //case WM_ACTIVATE:
            //	if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) target->OnActivate();
            //	else if (wParam == WA_INACTIVE) target->OnDeactivate();
            //	return 0;

        case WM_SETFOCUS:
            target->OnActivate();
            return 0;
        case WM_KILLFOCUS:
            target->OnDeactivate();
            return 0;
        case WM_KEYDOWN:
            target->OnKeyDown(wParam);
            return 0;
        case WM_INPUT:
            if (inputSystem) return inputSystem->ProcessInputMessage((HRAWINPUT)lParam);
            else return 0;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
