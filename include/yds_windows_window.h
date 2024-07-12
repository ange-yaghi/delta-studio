#ifndef YDS_WINDOWS_WINDOW_H
#define YDS_WINDOWS_WINDOW_H

#include "yds_window.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/windows.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif

class ysWindowsWindowSystem;

class ysWindowsWindow : public ysWindow {
    friend ysWindowsWindowSystem;

public:
    ysWindowsWindow();
    virtual ~ysWindowsWindow();

    // Overrides

    virtual ysError InitializeWindow(ysWindow *parent, const wchar_t *title,
                                     WindowStyle style, int x, int y, int width,
                                     int height, ysMonitor *monitor,
                                     WindowState initialState,
                                     const ysVector &color) override;
    virtual ysError InitializeWindow(ysWindow *parent, const wchar_t *title,
                                     WindowStyle style,
                                     ysMonitor *monitor) override;
    virtual void CaptureMouse() override;
    virtual void ReleaseMouse() override;

    virtual bool SetWindowStyle(WindowStyle style);

    virtual void ScreenToLocal(int &x, int &y) const;

    virtual bool IsVisible();

    virtual WindowState GetState() const override;
    virtual int GetScreenWidth() const override;
    virtual int GetScreenHeight() const override;

public:
    // Windows Specific

    inline void SetInstance(HINSTANCE instance) { m_instance = instance; }
    inline HINSTANCE GetInstance() const { return m_instance; }

    inline HWND GetWindowHandle() const { return m_hwnd; }

    static int GetWindowsStyle(WindowStyle style);
    static int GetWindowsState(WindowState state);

protected:
    // Abstraction Layer

    virtual void Close() override;
    virtual void SetTitle(const wchar_t *title) override;
    virtual void SetState(WindowState state = WindowState::Visible) override;

    virtual void AL_SetSize(int width, int height) override;
    virtual void AL_SetLocation(int x, int y) override;

protected:
    ATOM RegisterWindowsClass(const ysVector &color);

    HINSTANCE m_instance;
    HWND m_hwnd;
    int m_previousCmdShow;
};

#endif /* YDS_WINDOWS_WINDOW_H */
