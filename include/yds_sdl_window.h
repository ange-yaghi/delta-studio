#pragma once

#include "yds_window.h"

#include <SDL2/SDL_video.h>

class ysSdlWindowSystem;
class ysOpenGLSdlContext;

class ysSdlWindow : public ysWindow {
    friend ysSdlWindowSystem;
    friend ysOpenGLSdlContext;

public:
    ysSdlWindow();
    ~ysSdlWindow();

    // Overrides
    virtual ysError InitializeWindow(ysWindow *parent, std::string title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor, ysContextObject::DeviceAPI api) override;
    virtual void SetState(WindowState state) override;
    virtual void SetTitle(std::string title) override;
    virtual bool SetWindowStyle(WindowStyle style) override;

protected:
    SDL_Window *m_window = nullptr;
    int m_old_width = 0;
    int m_old_height = 0;
};
