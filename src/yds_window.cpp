#include "../include/yds_window.h"

#include "../include/yds_window_event_handler.h"
#include "../include/yds_window_system.h"

#include <cmath>

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

#include "safe_lib.h"

// TODO: -


#elif defined(_WIN64)

ysWindow::ysWindow() : ysWindowSystemObject("WINDOW", Platform::Unknown) {
    m_title[0] = '\0';

    m_monitor = nullptr;

    m_width = 0;
    m_height = 0;

    m_locationx = 0;
    m_locationy = 0;

    m_windowedWidth = 0;
    m_windowedHeight = 0;

    m_windowedLocationx = 0;
    m_windowedLocationy = 0;

    m_windowStyle = WindowStyle::Unknown;
    m_windowState = WindowState::Hidden;
    m_parent = nullptr;

    m_active = false;
    m_resizing = false;

    m_eventHandler = nullptr;

    m_gameResolutionScaleHorizontal = m_gameResolutionScaleVertical = 1.0f;
}

ysWindow::ysWindow(Platform platform)
    : ysWindowSystemObject("WINDOW", platform) {
    m_title[0] = '\0';

    m_monitor = nullptr;

    m_width = 0;
    m_height = 0;

    m_locationx = 0;
    m_locationy = 0;

    m_windowedWidth = 0;
    m_windowedHeight = 0;

    m_windowedLocationx = 0;
    m_windowedLocationy = 0;

    m_windowStyle = WindowStyle::Unknown;
    m_windowState = WindowState::Hidden;
    m_parent = nullptr;

    m_active = true;
    m_resizing = false;

    m_eventHandler = nullptr;
    m_backgroundColor = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 1.0f);

    m_gameResolutionScaleHorizontal = m_gameResolutionScaleVertical = 1.0f;
}

ysWindow::~ysWindow() {}

ysError ysWindow::InitializeWindow(ysWindow *parent, const wchar_t *title,
                                   WindowStyle style, int x, int y, int width,
                                   int height, ysMonitor *monitor,
                                   WindowState initialState, const ysVector &color) {
    YDS_ERROR_DECLARE("InitializeWindow");

    wcscpy_s(m_title, 256, title);

    m_width = width;
    m_height = height;

    m_locationx = x;
    m_locationy = y;

    m_windowedWidth = width;
    m_windowedHeight = height;

    m_windowedLocationx = x;
    m_windowedLocationy = y;

    m_windowState = initialState;
    m_windowStyle = style;

    m_backgroundColor = color;
    m_monitor = monitor;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysWindow::InitializeWindow(ysWindow *parent, const wchar_t *title,
                                   WindowStyle style, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    YDS_NESTED_ERROR_CALL(InitializeWindow(
            parent, title, style, monitor->GetOriginX(), monitor->GetOriginY(),
            monitor->GetPhysicalWidth(), monitor->GetPhysicalHeight(), monitor,
            WindowState::Visible));

    return YDS_ERROR_RETURN(ysError::None);
}

void ysWindow::RestoreWindow() {
    WindowState prevWindowState = m_windowState;

    InitializeWindow(m_parent, m_title, m_windowStyle, m_locationx, m_locationy,
                     m_width, m_height, m_monitor, prevWindowState);
    SetState(prevWindowState);
}

const int ysWindow::GetGameWidth() const {
    return int(std::round(m_gameResolutionScaleHorizontal * GetScreenWidth()));
}

const int ysWindow::GetGameHeight() const {
    return int(std::round(m_gameResolutionScaleVertical * GetScreenHeight()));
}

bool ysWindow::IsOnScreen(int x, int y) const {
    ScreenToLocal(x, y);
    return (x >= 0 && x <= GetScreenWidth()) &&
           (y >= 0 && y <= GetScreenHeight());
}

bool ysWindow::IsOpen() const { return m_windowState != WindowState::Closed; }

bool ysWindow::IsActive() { return m_active; }

bool ysWindow::IsVisible() { return false; }

void ysWindow::SetScreenSize(int width, int height) {
    OnResizeWindow(width, height);
    AL_SetSize(width, height);
}

void ysWindow::SetLocation(int x, int y) {
    OnMoveWindow(x, y);
    AL_SetLocation(x, y);
}

void ysWindow::SetWindowSize(int width, int height) {
    OnResizeWindow(width, height);
    AL_SetSize(width, height);
}

void ysWindow::SetTitle(const wchar_t *title) { wcscpy(m_title, title); }

void ysWindow::StartResizing() {
    m_resizing = true;
    if (m_eventHandler != nullptr) { m_eventHandler->OnStartMoveResize(); }
}

void ysWindow::EndResizing() {
    m_resizing = false;
    if (m_eventHandler != nullptr) { m_eventHandler->OnEndMoveResize(); }
}

bool ysWindow::SetWindowStyle(WindowStyle style) {
    if (style == m_windowStyle) return false;

    if (style == WindowStyle::Fullscreen) {
        //RaiseError(m_monitor != NULL, "Cannot go into fullscreen without an attached monitor.");
        m_windowStyle = style;
    } else if (style == WindowStyle::Windowed) {
        m_windowStyle = style;
    } else if (style == WindowStyle::Popup) {
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

void ysWindow::SetGameResolutionScale(float scale) {
    SetGameResolutionScaleHorizontal(scale);
    SetGameResolutionScaleVertical(scale);
}

void ysWindow::SetGameResolutionScaleHorizontal(float scale) {
    m_gameResolutionScaleHorizontal = scale;
    SetWindowSize(m_width, m_height);
}

void ysWindow::SetGameResolutionScaleVertical(float scale) {
    m_gameResolutionScaleVertical = scale;
    SetWindowSize(m_width, m_height);
}

void ysWindow::OnMoveWindow(int x, int y) {
    m_locationx = x;
    m_locationy = y;

    if (m_eventHandler != nullptr) m_eventHandler->OnMoveWindow(x, y);
}

void ysWindow::OnResizeWindow(int w, int h) {
    m_width = w;
    m_height = h;

    if (m_eventHandler != nullptr) m_eventHandler->OnResizeWindow(w, h);
}

void ysWindow::OnActivate() {
    m_active = true;

    if (m_eventHandler != nullptr) m_eventHandler->OnActivate();
}

void ysWindow::OnDeactivate() {
    m_active = false;

    if (m_eventHandler) m_eventHandler->OnDeactivate();
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

    if (GetCurrentWindowStyle() == WindowStyle::Windowed) {
        m_windowedWidth = width;
        m_windowedHeight = height;
    }
}

void ysWindow::AL_SetLocation(int x, int y) {
    m_locationx = x;
    m_locationy = y;

    if (GetCurrentWindowStyle() == WindowStyle::Windowed) {
        m_windowedLocationx = x;
        m_windowedLocationy = y;
    }
}

#endif /* Windows */
