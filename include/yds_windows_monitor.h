#ifndef YDS_WINDOWS_MONITOR_H
#define YDS_WINDOWS_MONITOR_H

#include "yds_monitor.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/windows.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif

class ysWindowsMonitor : public ysMonitor {
public:
    ysWindowsMonitor();
    ~ysWindowsMonitor();

    void Initialize(HMONITOR monitor) { m_handle = monitor; }

    HMONITOR Handle() const { return m_handle; }

private:
    HMONITOR m_handle;
};

#endif /* YDS_WINDOWS_MONITOR_H */
