#include "../include/yds_windows_window.h"

#include "../include/yds_windows_window_system.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

//#include "win32/windows_modular.h"

// TODO: -


#elif defined(_WIN64)

ysWindowsWindow::ysWindowsWindow() : ysWindow(Platform::Windows) {
    m_instance = 0;
    m_hwnd = 0;
    m_previousCmdShow = SW_SHOW;
}

ysWindowsWindow::~ysWindowsWindow() { /* void */
}

ATOM ysWindowsWindow::RegisterWindowsClass(const ysVector &color) {
    const ysVector c_srgb = ysColor::linearToSrgb(color);

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) (ysWindowsWindowSystem::WinProc);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_instance;
    wc.hIcon = LoadIcon(m_instance, L"IDI_ICON1");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =
            CreateSolidBrush(RGB((int) (ysMath::GetX(c_srgb) * 255.0f),
                                 (int) (ysMath::GetY(c_srgb) * 255.0f),
                                 (int) (ysMath::GetZ(c_srgb) * 255.0f)));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"GAME_ENGINE_WINDOW";
    wc.hIconSm = NULL;

    return RegisterClassEx(&wc);
}

bool rectInsideMonitor(const RECT &rect) {
    HMONITOR hMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    const RECT monitorRect = mi.rcMonitor;
    return rect.left <= monitorRect.right && rect.right >= monitorRect.left &&
           rect.top <= monitorRect.bottom && rect.bottom >= monitorRect.top;
}

ysError ysWindowsWindow::InitializeWindow(
        ysWindow *parent, const wchar_t *title, WindowStyle style, int x, int y,
        int width, int height, ysMonitor *monitor, WindowState initialState,
        const ysVector &color) {
    YDS_ERROR_DECLARE("InitializeWindow");

    if (!CheckCompatibility(parent)) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    RECT rect;
    rect.left = x;
    rect.right = x + width;
    rect.top = y;
    rect.bottom = y + height;
    const bool validRect = rectInsideMonitor(rect);
    if (!validRect) { style = WindowStyle::Windowed; }

    YDS_NESTED_ERROR_CALL(ysWindow::InitializeWindow(parent, title, style, x, y,
                                                     width, height, monitor,
                                                     initialState, color));

    ysWindowsWindow *parentWindow = static_cast<ysWindowsWindow *>(parent);

    const int win32Style =
            GetWindowsStyle(style) | GetWindowsState(initialState);
    const HWND parentHandle = (parentWindow) ? parentWindow->m_hwnd : NULL;

    RegisterWindowsClass(color);

    m_hwnd = CreateWindow(
            L"GAME_ENGINE_WINDOW", title, win32Style,
            (x == INT_MAX || !validRect) ? CW_USEDEFAULT : rect.left,
            (y == INT_MAX || !validRect) ? CW_USEDEFAULT : rect.top,
            (width == INT_MAX || !validRect) ? CW_USEDEFAULT
                                             : std::min(width, 8000),
            (height == INT_MAX || !validRect) ? CW_USEDEFAULT
                                              : std::min(height, 8000),
            parentHandle, NULL, m_instance, NULL);

    m_locationx = x;
    m_locationy = y;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysWindowsWindow::InitializeWindow(ysWindow *parent,
                                          const wchar_t *title,
                                          WindowStyle style,
                                          ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    YDS_NESTED_ERROR_CALL(InitializeWindow(
            parent, title, style, monitor->GetOriginX(), monitor->GetOriginY(),
            monitor->GetPhysicalWidth(), monitor->GetPhysicalHeight(), monitor,
            WindowState::Visible, {0.0f, 0.0f, 0.0f, 1.0f}));

    return YDS_ERROR_RETURN(ysError::None);
}

void ysWindowsWindow::ReleaseMouse() { ReleaseCapture(); }

void ysWindowsWindow::CaptureMouse() { SetCapture(m_hwnd); }

bool ysWindowsWindow::SetWindowStyle(WindowStyle style) {
    if (m_hwnd == NULL) { return false; }

    if (!ysWindow::SetWindowStyle(style)) { return false; }

    if (style == WindowStyle::Windowed) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle(style));
        SetWindowPos(m_hwnd, NULL, m_windowedLocationx, m_windowedLocationy,
                     m_windowedWidth, m_windowedHeight,
                     SWP_FRAMECHANGED | SWP_NOZORDER);

        ShowWindow(m_hwnd, m_previousCmdShow);
        SetForegroundWindow(m_hwnd);
    } else if (style == WindowStyle::Fullscreen) {
        RECT rect;
        GetWindowRect(m_hwnd, &rect);

        m_windowedLocationx = rect.left;
        m_windowedLocationy = rect.top;
        m_windowedWidth = rect.right - rect.left;
        m_windowedHeight = rect.bottom - rect.top;

        WINDOWPLACEMENT placement = {};
        GetWindowPlacement(m_hwnd, &placement);
        m_previousCmdShow = placement.showCmd;

        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle(style));
        SetWindowPos(m_hwnd, NULL, m_windowedLocationx, m_windowedLocationy,
                     m_windowedWidth, m_windowedHeight,
                     SWP_FRAMECHANGED | SWP_NOZORDER);

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetWindowSize(m_monitor->GetLogicalWidth(),
                      m_monitor->GetLogicalHeight());

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    } else if (style == WindowStyle::Popup) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle(style));
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }

    return true;
}

void ysWindowsWindow::ScreenToLocal(int &x, int &y) const {
    if (m_hwnd == NULL) {
        x = y = 0;
        return;
    }

    POINT p = {x, y};
    BOOL result = ScreenToClient(m_hwnd, &p);

    if (result == FALSE) {
        x = y = 0;
        return;
    }

    x = p.x;
    y = GetScreenHeight() - p.y;
}

bool ysWindowsWindow::IsVisible() {
    if (m_hwnd == NULL) return 0;

    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);
    const BOOL result = GetWindowPlacement(m_hwnd, &placement);
    if (result == FALSE) return false;

    switch (placement.showCmd) {
        case SW_HIDE:
            return false;
        case SW_MINIMIZE:
            return false;
        default:
            return true;
    }
}

ysWindowsWindow::WindowState ysWindowsWindow::GetState() const {
    if (m_hwnd == NULL) { return WindowState::Hidden; }

    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);
    const BOOL result = GetWindowPlacement(m_hwnd, &placement);
    if (result == FALSE) { return WindowState::Hidden; }

    switch (placement.showCmd) {
        case SW_HIDE:
            return WindowState::Hidden;
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
            return WindowState::Minimized;
        case SW_MAXIMIZE:
            return WindowState::Maximized;
        default:
            return WindowState::Visible;
    }
}

int ysWindowsWindow::GetScreenWidth() const {
    if (m_hwnd == NULL) return 0;

    RECT rect;
    BOOL result = GetClientRect(m_hwnd, &rect);

    return (result == TRUE) ? rect.right - rect.left : 0;
}

int ysWindowsWindow::GetScreenHeight() const {
    if (m_hwnd == NULL) return 0;

    RECT rect;
    BOOL result = GetClientRect(m_hwnd, &rect);

    return (result == TRUE) ? rect.bottom - rect.top : 0;
}

int ysWindowsWindow::GetWindowsStyle(WindowStyle style) {
    switch (style) {
        case WindowStyle::Fullscreen:
            return WS_POPUP;
        case WindowStyle::Windowed:
            return WS_OVERLAPPEDWINDOW;
        case WindowStyle::Popup:
            return WS_POPUP;
    }

    return WS_OVERLAPPED;
}

int ysWindowsWindow::GetWindowsState(WindowState state) {
    switch (state) {
        case WindowState::Closed:
            return 0;
        case WindowState::Hidden:
            return 0;
        case WindowState::Maximized:
            return WS_VISIBLE | WS_MAXIMIZE;
        case WindowState::Minimized:
            return WS_VISIBLE | WS_MINIMIZE;
        case WindowState::Visible:
            return WS_VISIBLE;
        default:
            return WS_VISIBLE;
    }
}

// Abstraction Layer

void ysWindowsWindow::Close() {
    ysWindow::Close();

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;
}

void ysWindowsWindow::SetTitle(const wchar_t *title) {
    ysWindow::SetTitle(title);

    SetWindowText(m_hwnd, title);
}

void ysWindowsWindow::SetState(WindowState state) {
    if (m_hwnd == NULL) return;

    ysWindow::SetState(state);

    switch (state) {
        case WindowState::Visible:
            ShowWindow(m_hwnd, SW_SHOW);
            break;
        case WindowState::Hidden:
            ShowWindow(m_hwnd, SW_HIDE);
            break;
        case WindowState::Maximized:
            ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
            break;
        case WindowState::Minimized:
            ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
            break;
    }

    SetForegroundWindow(m_hwnd);
    UpdateWindow(m_hwnd);
}

void ysWindowsWindow::AL_SetSize(int width, int height) {
    if (m_hwnd == NULL) return;

    ysWindow::AL_SetSize(width, height);

    int adjustedWidth = width, adjustedHeight = height;
    if (GetCurrentWindowStyle() == WindowStyle::Windowed) {
        RECT rect;
        rect.left = 0;
        rect.right = width;
        rect.top = 0;
        rect.bottom = height;
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        adjustedWidth = rect.right - rect.left;
        adjustedHeight = rect.bottom - rect.top;
    }

    SetWindowPos(m_hwnd, NULL, 0, 0, adjustedWidth, adjustedHeight,
                 SWP_NOMOVE | SWP_NOZORDER);
}

void ysWindowsWindow::AL_SetLocation(int x, int y) {
    if (m_hwnd == NULL) return;

    ysWindow::AL_SetLocation(x, y);

    int adjusted_x = x, adjusted_y = y;
    if (GetCurrentWindowStyle() == WindowStyle::Windowed) {
        RECT rect;
        rect.left = x;
        rect.right = 0;
        rect.top = y;
        rect.bottom = 0;
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        adjusted_x = rect.left;
        adjusted_y = rect.top;
    }

    POINT p = {x, y};
    ScreenToClient(m_hwnd, &p);
    SetWindowPos(m_hwnd, NULL, adjusted_x, adjusted_y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER);
}

#endif /* Windows */
