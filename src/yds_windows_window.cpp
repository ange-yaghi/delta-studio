#include "../include/yds_windows_window.h"

#include "../include/yds_windows_window_system.h"

ysWindowsWindow::ysWindowsWindow() : ysWindow(Platform::Windows) {
    m_instance = 0;
    m_hwnd = 0;
}

ysWindowsWindow::~ysWindowsWindow() {
    /* void */
}

ATOM ysWindowsWindow::RegisterWindowsClass() {
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)(ysWindowsWindowSystem::WinProc);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GAME_ENGINE_WINDOW";
    wc.hIconSm = NULL;

    return RegisterClassEx(&wc);
}

ysError ysWindowsWindow::InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    if (!CheckCompatibility(parent)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    YDS_NESTED_ERROR_CALL(ysWindow::InitializeWindow(parent, title, style, x, y, width, height, monitor));

    ysWindowsWindow *parentWindow = static_cast<ysWindowsWindow *>(parent);

    const int win32Style = GetWindowsStyle();
    HWND parentHandle = (parentWindow) 
        ? parentWindow->m_hwnd 
        : NULL;

    RegisterWindowsClass();

    m_hwnd = CreateWindow(
        "GAME_ENGINE_WINDOW",
        title,
        win32Style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        parentHandle,
        NULL,
        m_instance,
        NULL);

    m_locationx = x;
    m_locationy = y;

    SetWindowPos(m_hwnd, NULL, m_locationx, m_locationy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysWindowsWindow::InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    YDS_NESTED_ERROR_CALL(InitializeWindow(parent, title, style, monitor->GetOriginX(), monitor->GetOriginY(), monitor->GetPhysicalWidth(), monitor->GetPhysicalHeight(), monitor));

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysWindowsWindow::SetWindowStyle(WindowStyle style) {
    if (m_hwnd == NULL) return false;

    if (!ysWindow::SetWindowStyle(style)) return false;

    if (style == WindowStyle::Windowed) {
        const int width = GetScreenWidth();
        const int height = GetScreenHeight();

        const int x = GetPositionX();
        const int y = GetPositionY();

        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        SetLocation(x, y);
        SetWindowSize(width, height);

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }
    else if (style == WindowStyle::Fullscreen) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetWindowSize(m_monitor->GetPhysicalWidth(), m_monitor->GetPhysicalHeight());

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }
    else if (style == WindowStyle::Popup) {
        const int width = GetScreenWidth();
        const int height = GetScreenHeight();

        const int x = GetPositionX();
        const int y = GetPositionY();

        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        SetLocation(x, y);
        SetWindowSize(width, height);

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

    POINT p = { x, y };
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
    BOOL result = GetWindowPlacement(m_hwnd, &placement);
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

int ysWindowsWindow::GetScreenWidth() const {
    if (m_hwnd == NULL) return 0;

    RECT rect;
    BOOL result = GetClientRect(m_hwnd, &rect);

    return (result == TRUE)
        ? rect.right - rect.left
        : 0;
}

int ysWindowsWindow::GetScreenHeight() const {
    if (m_hwnd == NULL) return 0;

    RECT rect;
    BOOL result = GetClientRect(m_hwnd, &rect);

    return (result == TRUE)
        ? rect.bottom - rect.top
        : 0;
}

int ysWindowsWindow::GetWindowsStyle() const {
    switch (m_windowStyle) {
    case WindowStyle::Fullscreen:
        return WS_POPUP | WS_VISIBLE;
    case WindowStyle::Windowed:
        return WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    case WindowStyle::Popup:
        return WS_POPUP | WS_VISIBLE;
    }

    return WS_OVERLAPPED;
}

// Abstraction Layer

void ysWindowsWindow::Close() {
    ysWindow::Close();

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;
}

void ysWindowsWindow::SetTitle(const char *title) {
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

    SetWindowPos(m_hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void ysWindowsWindow::AL_SetLocation(int x, int y) {
    if (m_hwnd == NULL) return;

    ysWindow::AL_SetLocation(x, y);

    SetWindowPos(m_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
