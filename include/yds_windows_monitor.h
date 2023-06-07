#ifndef YDS_WINDOWS_MONITOR_H
#define YDS_WINDOWS_MONITOR_H

#include "yds_monitor.h"

#define NOMINMAX
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

#endif /* YDS_WINDOWS_MONITOR_H */
