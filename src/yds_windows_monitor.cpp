#include "../include/yds_windows_monitor.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    // #include "win32/windows_modular.h"
#elif defined(_WIN64)

#include <Windows.h>

ysWindowsMonitor::ysWindowsMonitor() : ysMonitor(Platform::Windows) {
    // FIXME: It's windows only
    InitializeDeviceName(CCHDEVICENAME);
}

ysWindowsMonitor::~ysWindowsMonitor() {
    /* void */
}

#endif /* Windwos */
