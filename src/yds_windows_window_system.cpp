#include "../include/yds_windows_window_system.h"

#include "../include/yds_windows_input_system.h"
#include "../include/yds_windows_monitor.h"
#include "../include/yds_windows_window.h"

#if defined(_WIN64)

#include <shellscalingapi.h>

ysWindowsWindowSystem::ysWindowsWindowSystem()
    : ysWindowSystem(Platform::Windows) {
    m_instance = NULL;
    m_oldCursor = NULL;
    m_oldCursorClip = RECT();
}

ysWindowsWindowSystem::~ysWindowsWindowSystem() { /* void */
}

ysError ysWindowsWindowSystem::NewWindow(ysWindow **newWindow) {
    YDS_ERROR_DECLARE("NewWindow");

    if (newWindow == nullptr)
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (m_instance == NULL) return YDS_ERROR_RETURN(ysError::NoContext);

    ysWindowsWindow *windowsWindow =
            m_windowArray.NewGeneric<ysWindowsWindow>();
    windowsWindow->SetInstance(m_instance);

    *newWindow = static_cast<ysWindow *>(windowsWindow);

    return YDS_ERROR_RETURN(ysError::None);
}

ysMonitor *ysWindowsWindowSystem::NewMonitor() {
    return m_monitorArray.NewGeneric<ysWindowsMonitor>();
}

ysMonitor *ysWindowsWindowSystem::MonitorFromWindow(ysWindow *window) {
    this->SurveyMonitors();

    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(window);
    HMONITOR monitorHandle = ::MonitorFromWindow(
            windowsWindow->GetWindowHandle(), MONITOR_DEFAULTTONEAREST);
    return FindMonitorFromHandle(monitorHandle);
}

BOOL CALLBACK MonitorEnum(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor,
                          LPARAM dwData) {
    ysWindowsWindowSystem *windowSystem =
            reinterpret_cast<ysWindowsWindowSystem *>(dwData);
    ysMonitor *monitorObject = windowSystem->FindMonitorFromHandle(hMonitor);

    if (monitorObject == nullptr) {
        monitorObject = windowSystem->NewMonitor();
    }

    MONITORINFOEX info;
    info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &info);

    static_cast<ysWindowsMonitor *>(monitorObject)->Initialize(hMonitor);
    monitorObject->SetOrigin(lprcMonitor->left, lprcMonitor->top);
    monitorObject->SetLogicalSize(lprcMonitor->right - lprcMonitor->left,
                                  lprcMonitor->bottom - lprcMonitor->top);
    monitorObject->SetDeviceName(info.szDevice);

    DEVMODE devMode;
    devMode.dmSize = sizeof(devMode);
    devMode.dmDriverExtra = 0;
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

    monitorObject->SetPhysicalSize(devMode.dmPelsWidth, devMode.dmPelsHeight);
    monitorObject->CalculateScaling();
    monitorObject->SetConnected(true);

    return TRUE;
}

ysError ysWindowsWindowSystem::SurveyMonitors() {
    YDS_ERROR_DECLARE("SurveyMonitors");
    YDS_NESTED_ERROR_CALL(ysWindowSystem::SurveyMonitors());

    {
        const int monitorCount = m_monitorArray.GetNumObjects();
        for (int i = 0; i < monitorCount; ++i) {
            m_monitorArray.Get(i)->SetConnected(false);
        }
    }

    const BOOL result =
            EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM) this);
    if (result == FALSE) {
        return YDS_ERROR_RETURN(ysError::CouldNotEnumerateMonitors);
    }

    return YDS_ERROR_RETURN(ysError::None);
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
        ysWindowsWindow *windowsWindow =
                static_cast<ysWindowsWindow *>(m_windowArray.Get(i));

        if (windowsWindow->GetWindowHandle() == handle) return windowsWindow;
    }

    return nullptr;
}

void ysWindowsWindowSystem::ConnectInstance(void *genericInstanceConnection) {
    HINSTANCE *instance = static_cast<HINSTANCE *>(genericInstanceConnection);

    SetProcessDPIAware();

    m_instance = *instance;
}

LRESULT WINAPI ysWindowsWindowSystem::WinProc(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam) {
    const DWORD id = GetCurrentThreadId();

    ysWindowSystem *system = ysWindowSystem::Get();

    ysWindowsWindowSystem *windowsSystem =
            static_cast<ysWindowsWindowSystem *>(system);
    ysWindowsInputSystem *inputSystem = static_cast<ysWindowsInputSystem *>(
            windowsSystem->GetInputSystem());

    ysWindow *target = windowsSystem->FindWindowFromHandle(hWnd);

    PAINTSTRUCT ps;
    HDC hdc;

    if (target != nullptr) {
        switch (msg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_ERASEBKGND: {
                const ysVector color =
                        ysColor::linearToSrgb(target->GetBackgroundColor());
                HBRUSH brush = CreateSolidBrush(
                        RGB((int) (ysMath::GetX(color) * 255.0f),
                            (int) (ysMath::GetY(color) * 255.0f),
                            (int) (ysMath::GetZ(color) * 255.0f)));

                RECT rc;
                GetClientRect(hWnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc, brush);
                return TRUE;
            }
            case WM_PAINT:
                hdc = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                break;
            case WM_CLOSE:
                target->OnCloseWindow();
                ysWindowSystem::Get()->CloseWindow(target);
                return 0;
            case WM_SIZE:
                if (!target->IsResizing()) {
                    target->OnResizeWindow(LOWORD(lParam), HIWORD(lParam));
                }
                return 0;
            case WM_ENTERSIZEMOVE:
                target->StartResizing();
                return 0;
            case WM_EXITSIZEMOVE: {
                RECT rect;
                if (GetClientRect(hWnd, &rect)) {
                    target->OnResizeWindow(rect.right - rect.left,
                                           rect.bottom - rect.top);
                }

                target->EndResizing();

                return 0;
            }
            case WM_MOVE: {
                const int x = int(short(LOWORD(lParam)));
                const int y = int(short(HIWORD(lParam)));
                target->OnMoveWindow(x, y);
                return 0;
            }
            case WM_SETFOCUS:
                target->OnActivate();
                return 0;
            case WM_KILLFOCUS:
                target->OnDeactivate();
                return 0;
            case WM_INPUT:
                if (inputSystem != nullptr) {
                    if (inputSystem->IsGlobalInputEnabled() ||
                        target->IsActive()) {
                        return inputSystem->ProcessInputMessage(
                                (HRAWINPUT) lParam);
                    } else {
                        return 0;
                    }
                } else {
                    return 0;
                }
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    system->SetCursor(system->GetCursor());
                    return TRUE;
                }
        }

        if (inputSystem != nullptr) {
            /* if (inputSystem->IsGlobalInputEnabled() || target->IsActive()) */
            {
                switch (msg) {
                    case WM_KEYDOWN:
                    case WM_KEYUP:
                    case WM_SYSKEYDOWN:
                    case WM_SYSKEYUP:
                        return inputSystem->OnOsKey(lParam, wParam);
                    case WM_RBUTTONDOWN:
                        return inputSystem->OnOsMouseButtonDown(
                                ysMouse::Button::Right);
                    case WM_RBUTTONUP:
                        return inputSystem->OnOsMouseButtonUp(
                                ysMouse::Button::Right);
                    case WM_LBUTTONDOWN:
                        return inputSystem->OnOsMouseButtonDown(
                                ysMouse::Button::Left);
                    case WM_LBUTTONUP:
                        return inputSystem->OnOsMouseButtonUp(
                                ysMouse::Button::Left);
                    case WM_MBUTTONDOWN:
                        return inputSystem->OnOsMouseButtonDown(
                                ysMouse::Button::Middle);
                    case WM_MBUTTONUP:
                        return inputSystem->OnOsMouseButtonUp(
                                ysMouse::Button::Middle);
                    case WM_MOUSEWHEEL:
                        return inputSystem->OnOsMouseWheel(lParam, wParam);
                    case WM_MOUSEMOVE:
                        return inputSystem->OnOsMouseMove(lParam, wParam);
                }
            }
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ysWindowsWindowSystem::ConfineCursor(ysWindow *window) {
    if (IsCursorConfined()) return;
    ysWindowSystem::ConfineCursor(window);

    GetClipCursor(&m_oldCursorClip);
    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(window);

    RECT clientRect;
    GetClientRect(windowsWindow->GetWindowHandle(), &clientRect);

    POINT ul, lr;
    ul.x = clientRect.left;
    ul.y = clientRect.top;
    lr.x = clientRect.right;
    lr.y = clientRect.bottom;

    MapWindowPoints(windowsWindow->GetWindowHandle(), nullptr, &ul, 1);
    MapWindowPoints(windowsWindow->GetWindowHandle(), nullptr, &lr, 1);

    clientRect.left = ul.x;
    clientRect.top = ul.y;

    clientRect.right = lr.x;
    clientRect.bottom = lr.y;

    ClipCursor(&clientRect);
}

void ysWindowsWindowSystem::ReleaseCursor() {
    if (!IsCursorConfined()) return;
    ysWindowSystem::ReleaseCursor();

    ClipCursor(&m_oldCursorClip);
}

void ysWindowsWindowSystem::SetCursorPosition(int x, int y) {
    SetCursorPos(x, y);
}

void ysWindowsWindowSystem::SetCursorVisible(bool visible) {
    ysWindowSystem::SetCursorVisible(visible);

    m_oldCursor = ::GetCursor();

    int displayCount = ShowCursor(visible ? TRUE : FALSE);

    // ShowCursor() increments and returns a "display counter" which
    // must be greater than or equal to 0 in order for the cursor
    // to be displayed. The code below ensures that the cursor will be
    // displayed or hidden by forcing the final value of the counter
    if (visible) {
        while (displayCount < 0) { displayCount = ShowCursor(TRUE); }
    } else {
        while (displayCount >= 0) { displayCount = ShowCursor(FALSE); }
    }

    // This step may not be necessary but doesn't hurt anything
    if (visible) { ::SetCursor(m_oldCursor); }
}

void ysWindowsWindowSystem::SetCursor(Cursor cursor) {
    ysWindowSystem::SetCursor(cursor);

    LPCTSTR winapiCursor = IDC_ARROW;
    switch (cursor) {
        case Cursor::Hand:
            winapiCursor = IDC_HAND;
            break;
        case Cursor::Pan:
            winapiCursor = IDC_SIZEALL;
            break;
        case Cursor::Arrow:
            winapiCursor = IDC_ARROW;
            break;
        case Cursor::Blocked:
            winapiCursor = IDC_NO;
            break;
    }

    ::SetCursor(LoadCursor(NULL, winapiCursor));
}

ysMonitor *ysWindowsWindowSystem::FindMonitorFromHandle(HMONITOR handle) {
    const int monitorCount = m_monitorArray.GetNumObjects();
    for (int i = 0; i < monitorCount; ++i) {
        ysWindowsMonitor *monitor =
                static_cast<ysWindowsMonitor *>(m_monitorArray.Get(i));
        if (monitor->Handle() == handle) { return monitor; }
    }

    return nullptr;
}

#endif /* Windows */
