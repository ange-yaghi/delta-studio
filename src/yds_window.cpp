#include "../include/yds_window.h"

#include "../include/yds_window_event_handler.h"
#include "../include/yds_window_system.h"

ysWindow::ysWindow() : ysWindowSystemObject("WINDOW", Platform::UNKNOWN) {
    m_title[0] = '\0';

    m_monitor = nullptr;

    m_width = 0;
    m_height = 0;
    m_frameHeightOffset = 0;
    m_frameWidthOffset = 0;
    m_frameOriginXOffset = 0;
    m_frameOriginYOffset = 0;

    m_locationx = 0;
    m_locationy = 0;

    m_windowStyle = WindowStyle::Unknown;
    m_windowState = WindowState::HIDDEN;
    m_parent = nullptr;

    m_active = false;

    m_eventHandler = nullptr;
}

ysWindow::ysWindow(Platform platform) : ysWindowSystemObject("WINDOW", platform) {
    m_title[0] = '\0';

    m_monitor = nullptr;

    m_width = 0;
    m_height = 0;
    m_frameHeightOffset = 0;
    m_frameWidthOffset = 0;
    m_frameOriginXOffset = 0;
    m_frameOriginYOffset = 0;

    m_locationx = 0;
    m_locationy = 0;

    m_windowStyle = WindowStyle::Unknown;
    m_windowState = WindowState::HIDDEN;
    m_parent = nullptr;

    m_active = false;

    m_eventHandler = nullptr;
}

ysWindow::~ysWindow() {
    /* void */
}

ysError ysWindow::InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    m_width = width;
    m_height = height;

    strcpy_s(m_title, 256, title);

    m_locationx = x;
    m_locationy = y;

    m_windowState = WindowState::HIDDEN; // DEFAULT
    m_windowStyle = style;

    m_monitor = monitor;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysWindow::InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    YDS_NESTED_ERROR_CALL(
        InitializeWindow(
            parent, title, style, 
            monitor->GetOriginX(), monitor->GetOriginY(), 
            monitor->GetWidth(), monitor->GetHeight(), monitor));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysWindow::RestoreWindow() {
    WindowState prevWindowState = m_windowState;

    InitializeWindow(m_parent, m_title, m_windowStyle, m_locationx, m_locationy, m_width, m_height, m_monitor);
    SetState(prevWindowState);
}

bool ysWindow::IsOpen() {
    return m_windowState != WindowState::CLOSED;
}

bool ysWindow::IsActive() {
    return m_active;
}

bool ysWindow::IsVisible() {
    return false;
}

void ysWindow::SetSize(int width, int height) {
    OnResizeWindow(width, height);
    AL_SetSize(width, height);
}

void ysWindow::SetLocation(int x, int y) {
    OnMoveWindow(x, y);
    AL_SetLocation(x, y);
}

void ysWindow::SetTitle(const char *title) {
    strcpy_s(m_title, 256, title);
}

bool ysWindow::SetWindowStyle(WindowStyle style) {
    if (style == m_windowStyle) return false;

    if (style == WindowStyle::FULLSCREEN) {
        //RaiseError(m_monitor != NULL, "Cannot go into fullscreen without an attached monitor.");
        m_windowStyle = style;
    }
    else if (style == WindowStyle::WINDOWED) {
        m_windowStyle = style;
    }
    else if (style == WindowStyle::POPUP) {
        m_windowStyle = style;
    }

    return true;
}

// Handlers

void ysWindow::AttachEventHandler(ysWindowEventHandler *handler) {
    if (m_eventHandler) m_eventHandler->m_window = NULL;
    if (handler) handler->m_window = this;

    m_eventHandler = handler;
}

void ysWindow::OnMoveWindow(int x, int y) {
    m_locationx = x + m_frameOriginXOffset;
    m_locationy = y + m_frameOriginYOffset;

    if (m_eventHandler) m_eventHandler->OnMoveWindow(x, y);
}

void ysWindow::OnResizeWindow(int w, int h) {
    m_width = w + m_frameWidthOffset;
    m_height = h + m_frameHeightOffset;

    if (m_eventHandler) m_eventHandler->OnResizeWindow(w, h);
}

void ysWindow::OnActivate() {
    m_active = true;

    if (m_eventHandler) m_eventHandler->OnActivate();
}

void ysWindow::OnDeactivate() {
    //HWND current_active = GetFocus();

    m_active = false;

    if (m_eventHandler) m_eventHandler->OnDeactivate();

    //SetFocus(current_active);
}

void ysWindow::OnCloseWindow() {
    if (m_eventHandler) m_eventHandler->OnCloseWindow();
}

void ysWindow::OnKeyDown(int key) {
    if (m_eventHandler) m_eventHandler->OnKeyDown(key);
}

// Abstraction Layer

void ysWindow::AL_SetSize(int width, int height) {
    m_width = width;
    m_height = height;
}

void ysWindow::AL_SetLocation(int x, int y) {
    m_locationx = x;
    m_locationy = y;
}
