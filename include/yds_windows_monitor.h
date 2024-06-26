#ifndef YDS_WINDOWS_MONITOR_H
#define YDS_WINDOWS_MONITOR_H

#include "yds_monitor.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

//#include <Cocoa/Cocoa.h>
//
//class ysMacMonitor : public ysMonitor {
//public:
//    ysMacMonitor();
//    ~ysMacMonitor();
//
//    void Initialize(NSScreen *screen) { m_screen = screen; }
//
//    NSScreen *Screen() const { return m_screen; }
//
//private:
//    NSScreen *m_screen;
//};

/// Giving the same windows-name for a macOS target
//#define ysWindowsMonitor       ysMacMonitor

#elif defined(_WIN64)

#include <Windows.h>

class ysWindowsMonitor : public ysMonitor {
public:
    ysWindowsMonitor();
    ~ysWindowsMonitor();

    void Initialize(HMONITOR monitor) { m_handle = monitor; }

    HMONITOR Handle() const { return m_handle; }

private:
    HMONITOR m_handle;
};

#endif /* Windows */

#endif /* YDS_WINDOWS_MONITOR_H */
