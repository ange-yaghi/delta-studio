#include "../include/yds_windows_monitor.h"

#include <Windows.h>

ysWindowsMonitor::ysWindowsMonitor() : ysMonitor(Platform::Windows) {
    InitializeDeviceName(CCHDEVICENAME);
}

ysWindowsMonitor::~ysWindowsMonitor() {
    /* void */
}
