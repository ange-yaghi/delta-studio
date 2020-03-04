#ifndef YDS_WINDOW_H
#define YDS_WINDOW_H

#include "yds_window_system_object.h"
#include "yds_monitor.h"

class ysWindowEventHandler;
class ysWindowSystem;

class ysWindow : public ysWindowSystemObject {
public:
    static const int MAX_NAME_LENGTH = 256;

    enum class WindowState {
        VISIBLE,
        HIDDEN,
        MAXIMIZED,
        MINIMIZED,
        CLOSED,
        Unknown
    };

    enum class WindowStyle {
        WINDOWED,
        FULLSCREEN,
        POPUP,
        Unknown
    };

public:
    ysWindow();
    ysWindow(Platform platform);
    virtual ~ysWindow();

    /* Interface */
    virtual ysError InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, int x, int y, int width, int height, ysMonitor *monitor);
    virtual ysError InitializeWindow(ysWindow *parent, const char *title, WindowStyle style, ysMonitor *monitor);

    virtual void Close() { SetState(WindowState::CLOSED); }
    virtual void SetState(WindowState state = WindowState::VISIBLE) { m_windowState = state; }

    void RestoreWindow();

    int GetWidth()			const { return m_width; }
    int GetHeight()			const { return m_height; }
    int GetScreenWidth()	const { return m_width - m_frameWidthOffset; }
    int GetScreenHeight()	const { return m_height - m_frameHeightOffset; }
    WindowStyle GetStyle() const { return m_windowStyle; }
    ysMonitor *GetMonitor() const { return m_monitor; }

    bool IsOpen();
    virtual bool IsActive();
    virtual bool IsVisible();

    virtual void SetSize(int width, int height);
    virtual void SetLocation(int x, int y);
    virtual void SetTitle(const char *title);

    virtual bool SetWindowStyle(WindowStyle style);

    void AttachEventHandler(ysWindowEventHandler *handler);

public:
    // Handlers
    void OnMoveWindow(int x, int y);
    void OnResizeWindow(int w, int h);
    void OnActivate();
    void OnDeactivate();
    void OnCloseWindow();
    void OnKeyDown(int key);

protected:
    // Abstraction Layer
    virtual void AL_SetSize(int width, int height);
    virtual void AL_SetLocation(int x, int y);

protected:
    /* Common Members */

    // Window width
    int m_width;

    // Window height
    int m_height;

    // Location x
    int m_locationx;

    // Location y
    int m_locationy;

    // Frame Origin Offset
    int m_frameOriginXOffset;

    // Frame Origin Offset
    int m_frameOriginYOffset;

    // Frame Width Offset
    int m_frameWidthOffset;

    // Frame Height Offset
    int m_frameHeightOffset;

    // Title
    char m_title[MAX_NAME_LENGTH];

    // Current Window State
    WindowState m_windowState;

    // Window Style
    WindowStyle m_windowStyle;

    // Parent Window
    ysWindow *m_parent;

    // The attached monitor for use in fullscreen
    ysMonitor *m_monitor;

    // Window Active
    bool m_active;

protected:
    // Event handler
    ysWindowEventHandler *m_eventHandler;
};

#endif /* YDS_WINDOW_H */
