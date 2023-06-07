#include "../include/yds_monitor.h"

#include <cmath>
#include <wchar.h>

ysMonitor::ysMonitor() : ysWindowSystemObject("DISPLAY_MONITOR", Platform::Windows) {
    m_maxDeviceNameLength = 0;
    m_deviceName = 0;

    m_originx = 0;
    m_originy = 0;

    m_physicalWidth = 0;
    m_physicalHeight = 0;

    m_logicalWidth = 0;
    m_logicalHeight = 0;
    m_connected = false;
}

ysMonitor::ysMonitor(Platform platform) : ysWindowSystemObject("DISPLAY_MONITOR", platform) {
    m_maxDeviceNameLength = 0;
    m_deviceName = 0;

    m_originx = 0;
    m_originy = 0;

    m_physicalWidth = 0;
    m_physicalHeight = 0;

    m_logicalWidth = 0;
    m_logicalHeight = 0;
    m_connected = false;
}

ysMonitor::~ysMonitor() {
    if (m_deviceName) delete[] m_deviceName;
}

void ysMonitor::InitializeDeviceName(int maxLength) {
    m_maxDeviceNameLength = maxLength;
    m_deviceName = new wchar_t[maxLength];
}

void ysMonitor::SetOrigin(int x, int y) {
    m_originx = x;
    m_originy = y;
}

void ysMonitor::SetLogicalSize(int w, int h) {
    m_logicalWidth = w;
    m_logicalHeight = h;
}

void ysMonitor::SetPhysicalSize(int w, int h) {
    m_physicalWidth = w;
    m_physicalHeight = h;
}

void ysMonitor::SetDeviceName(const wchar_t *deviceName) {
    //RaiseError(m_deviceName != NULL, "Monitor device name is not initialized.");
    wcscpy(m_deviceName, deviceName);
}

void ysMonitor::CalculateScaling() {
    m_horizontalScaling = (float)m_logicalWidth / (float)m_physicalWidth;
    m_verticalScaling = (float)m_logicalHeight / (float)m_physicalHeight;
}

void ysMonitor::PhysicalToLogical(int px, int py, int &lx, int &ly) const {
    lx = (int)std::round(px * m_horizontalScaling);
    ly = (int)std::round(py * m_verticalScaling);
}

void ysMonitor::LogicalToPhysical(int lx, int ly, int &px, int &py) const {
    px = (int)std::round(lx / m_horizontalScaling);
    py = (int)std::round(ly / m_verticalScaling);
}
