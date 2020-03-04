#include "../include/yds_monitor.h"

ysMonitor::ysMonitor() : ysWindowSystemObject("DISPLAY_MONITOR", Platform::WINDOWS) {
    m_maxDeviceNameLength = 0;
    m_deviceName = 0;

    m_originx = 0;
    m_originy = 0;

    m_width = 0;
    m_height = 0;
}

ysMonitor::ysMonitor(Platform platform) : ysWindowSystemObject("DISPLAY_MONITOR", platform) {
    m_maxDeviceNameLength = 0;
    m_deviceName = 0;

    m_originx = 0;
    m_originy = 0;

    m_width = 0;
    m_height = 0;
}

ysMonitor::~ysMonitor() {
    if (m_deviceName) delete[] m_deviceName;
}

void ysMonitor::InitializeDeviceName(int maxLength) {
    m_maxDeviceNameLength = maxLength;
    m_deviceName = new char[maxLength];
}

void ysMonitor::SetOrigin(int x, int y) {
    m_originx = x;
    m_originy = y;
}

void ysMonitor::SetSize(int w, int h) {
    m_width = w;
    m_height = h;
}

void ysMonitor::SetDeviceName(const char *deviceName) {
    //RaiseError(m_deviceName != NULL, "Monitor device name is not initialized.");
    strcpy_s(m_deviceName, m_maxDeviceNameLength, deviceName);
}
