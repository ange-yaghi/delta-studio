#include "../include/yds_sdl_window.h"

ysSdlWindow::ysSdlWindow() : ysWindow(Platform::Sdl) {
    /* void */
}

ysSdlWindow::~ysSdlWindow() {
    /* void */
}

ysError ysSdlWindow::InitializeWindow(ysWindow *parent, std::string title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor, ysContextObject::DeviceAPI api) {
    YDS_ERROR_DECLARE("InitializeWindow");

    if (!CheckCompatibility(parent)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    YDS_NESTED_ERROR_CALL(ysWindow::InitializeWindow(parent, title, style, x, y, width, height, monitor, api));
    auto *parentWindow = static_cast<ysSdlWindow *>(parent);

    Uint32 flags = 0;

    if (m_windowState == WindowState::Hidden)
        flags |= SDL_WINDOW_HIDDEN;

    switch (style) {
    case WindowStyle::Windowed:
        flags |= SDL_WINDOW_RESIZABLE;
        break;
    case WindowStyle::Fullscreen:
        flags |= SDL_WINDOW_FULLSCREEN;
        break;
    case WindowStyle::Popup:
        flags |= SDL_WINDOW_BORDERLESS;
        break;
    case WindowStyle::Unknown:
        // do nothing
        break;
    }

    switch (api) {
    case ysContextObject::DeviceAPI::OpenGL4_0:
        flags |= SDL_WINDOW_OPENGL;
        break;
    case ysContextObject::DeviceAPI::Vulkan:
        flags |= SDL_WINDOW_VULKAN;
        break;
//    case ysContextObject::DeviceAPI::Metal:
//        flags |= SDL_WINDOW_METAL;
//        flags |= SDL_WINDOW_ALLOW_HIGHDPI;
//        break;
    default:
        break;
    }

    m_window = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysSdlWindow::SetState(WindowState state) {
    if (state == m_windowState)
        return;

    ysWindow::SetState(state);

    switch (state) {
    case WindowState::Visible:
        SDL_ShowWindow(m_window);
        break;
    case WindowState::Hidden:
        SDL_HideWindow(m_window);
        break;
    case WindowState::Maximized:
        SDL_MaximizeWindow(m_window);
        break;
    case WindowState::Minimized:
        SDL_MinimizeWindow(m_window);
        break;
    case WindowState::Closed:
        SDL_DestroyWindow(m_window);
        break;
    case WindowState::Unknown:
        // do nothing
        break;
    }
}

bool ysSdlWindow::SetWindowStyle(WindowStyle style) {
    const bool wasFullscreen = GetWindowStyle() == WindowStyle::Fullscreen;

    if (!ysWindow::SetWindowStyle(style)) return false;

    if (style == WindowStyle::Windowed) {
        const int width = wasFullscreen ? m_old_width : GetScreenWidth();
        const int height = wasFullscreen ? m_old_height : GetScreenHeight();

        const int x = GetPositionX();
        const int y = GetPositionY();

        SetLocation(x, y);
        SetWindowSize(width, height);

        SDL_SetWindowFullscreen(m_window, 0);
        SDL_SetWindowSize(m_window, width, height);
        SDL_RaiseWindow(m_window);
    } else if (style == WindowStyle::Fullscreen) {
        m_old_width = GetScreenWidth();
        m_old_height = GetScreenHeight();

        const int width = m_monitor->GetPhysicalWidth();
        const int height = m_monitor->GetPhysicalHeight();

        SetLocation(m_monitor->GetOriginX(), m_monitor->GetOriginY());
        SetWindowSize(width, height);

        SDL_SetWindowSize(m_window, width, height);
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    }

    return true;
}

void ysSdlWindow::SetTitle(std::string title) {
    SDL_SetWindowTitle(m_window, title.c_str());
}
