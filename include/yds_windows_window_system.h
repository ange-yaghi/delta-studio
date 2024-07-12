#ifndef YDS_WINDOWS_WINDOW_SYSTEM_H
#define YDS_WINDOWS_WINDOW_SYSTEM_H

#include "yds_window_system.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/window.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif


class ysWindowsWindowSystem : public ysWindowSystem {
public:
    ysWindowsWindowSystem();
    ~ysWindowsWindowSystem();

    virtual ysError NewWindow(ysWindow **newWindow) override;

    virtual ysMonitor *NewMonitor() override;
    virtual ysMonitor *MonitorFromWindow(ysWindow *window) override;
    virtual ysError SurveyMonitors() override;

    virtual void ProcessMessages();

    ysWindow *FindWindowFromHandle(HWND handle);

    virtual void ConnectInstance(void *genericInstanceConnection);

    static LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    virtual void ConfineCursor(ysWindow *window);
    virtual void ReleaseCursor();

    virtual void SetCursorPosition(int x, int y);
    virtual void SetCursorVisible(bool visible);

    virtual void SetCursor(Cursor cursor);

    ysMonitor *FindMonitorFromHandle(HMONITOR handle);

protected:
    HINSTANCE m_instance;

    RECT m_oldCursorClip;
    HCURSOR m_oldCursor;
};

#endif /* YDS_WINDOWS_WINDOW_SYSTEM_H */
