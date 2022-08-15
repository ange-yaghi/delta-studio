#pragma once

#include "yds_window.h"

#include <SDL2/SDL_video.h>

class ysSdlWindowSystem;
class ysOpenGLSDLContext;

class ysSdlWindow : public ysWindow {
    friend ysSdlWindowSystem;
    friend ysOpenGLSDLContext;

public:
    ysSdlWindow();
    ~ysSdlWindow();

    // Overrides
    virtual ysError InitializeWindow(ysWindow *parent, std::string title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor) override;
    virtual void SetState(WindowState state) override;
    virtual void SetTitle(std::string title) override;

protected:

    SDL_Window *m_window;

    WindowState m_appliedState = WindowState::Unknown;
};
