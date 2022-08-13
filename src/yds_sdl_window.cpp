#include "../include/yds_sdl_window.h"

ysSdlWindow::ysSdlWindow() : ysWindow(Platform::Sdl) {
    /* void */
}

ysSdlWindow::~ysSdlWindow() {
    /* void */
}

ysError ysSdlWindow::InitializeWindow(ysWindow *parent, std::string title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor) {
    YDS_ERROR_DECLARE("InitializeWindow");

    if (!CheckCompatibility(parent)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    YDS_NESTED_ERROR_CALL(ysWindow::InitializeWindow(parent, title, style, x, y, width, height, monitor));
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

    // TODO: choose between VULKAN and OPENGL here
    m_window = SDL_CreateWindow(title.c_str(), x, y, width, height, SDL_WINDOW_OPENGL);

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

    m_appliedState = state;
}

void ysSdlWindow::SetTitle(std::string title) {
    SDL_SetWindowTitle(m_window, title.c_str());
}
