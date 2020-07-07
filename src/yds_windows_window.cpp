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
    //RaiseError(m_instance != NULL, "Window instance not set.");

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

    if (!CheckCompatibility(parent)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

    YDS_NESTED_ERROR_CALL(ysWindow::InitializeWindow(parent, title, style, x, y, width, height, monitor));

    ysWindowsWindow *parentWindow = static_cast<ysWindowsWindow *>(parent);

    int win32Style = GetWindowsStyle();
    HWND parentHandle = (parentWindow) ? parentWindow->m_hwnd : NULL;

    RegisterWindowsClass();

    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, win32Style, FALSE);

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

    m_width = width;
    m_height = height;

    m_locationx = x;
    m_locationy = y;

    m_frameOriginXOffset = rc.left;
    m_frameOriginYOffset = rc.top;

    m_frameWidthOffset = rc.right - rc.left - width;
    m_frameHeightOffset = rc.bottom - rc.top - height;

    SetWindowPos(m_hwnd, NULL, m_locationx, m_locationy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysWindowsWindow::InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    YDS_NESTED_ERROR_CALL(InitializeWindow(parent, title, style, monitor->GetOriginX(), monitor->GetOriginY(), monitor->GetWidth(), monitor->GetHeight(), monitor));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

bool ysWindowsWindow::SetWindowStyle(WindowStyle style) {
    if (!ysWindow::SetWindowStyle(style)) return false;

    if (style == WindowStyle::WINDOWED) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        m_frameWidthOffset = 0;
        m_frameHeightOffset = 0;

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetSize(m_monitor->GetWidth(), m_monitor->GetHeight());

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }
    else if (style == WindowStyle::FULLSCREEN) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        m_frameWidthOffset = 0;
        m_frameHeightOffset = 0;

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetSize(m_monitor->GetWidth(), m_monitor->GetHeight());

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }
    else if (style == WindowStyle::POPUP) {
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowsStyle());
        SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED);

        m_frameWidthOffset = 0;
        m_frameHeightOffset = 0;

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetSize(m_monitor->GetWidth(), m_monitor->GetHeight());

        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
    }

    return true;
}

bool ysWindowsWindow::IsVisible() {
    WINDOWPLACEMENT placement;
    GetWindowPlacement(m_hwnd, &placement);

    switch (placement.showCmd) {
    case SW_HIDE:
        return false;
    case SW_MINIMIZE:
        return false;
    default:
        return true;
    }
}

int ysWindowsWindow::GetWindowsStyle() const {
    switch (m_windowStyle) {
    case WindowStyle::FULLSCREEN:
        return WS_POPUP | WS_VISIBLE;
    case WindowStyle::WINDOWED:
        return WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    case WindowStyle::POPUP:
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
    ysWindow::SetState(state);

    switch (state) {
    case WindowState::VISIBLE:
        ShowWindow(m_hwnd, SW_SHOW);
        break;
    case WindowState::HIDDEN:
        ShowWindow(m_hwnd, SW_HIDE);
        break;
    case WindowState::MAXIMIZED:
        ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
        break;
    case WindowState::MINIMIZED:
        ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
        break;
    }

    SetForegroundWindow(m_hwnd);
    UpdateWindow(m_hwnd);
}

void ysWindowsWindow::AL_SetSize(int width, int height) {
    ysWindow::AL_SetSize(width, height);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_width, m_height, SWP_NOMOVE | SWP_NOZORDER);
}

void ysWindowsWindow::AL_SetLocation(int x, int y) {
    ysWindow::AL_SetLocation(x, y);

    SetWindowPos(m_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
