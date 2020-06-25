#include "../include/yds_windows_input_device.h"

ysWindowsInputDevice::ysWindowsInputDevice() : ysInputDevice(Platform::Windows, InputDeviceType::UNKNOWN) {
    m_deviceHandle = NULL;
    memset(&m_info, 0, sizeof(m_info));
    m_systemName[0] = 0;
}

ysWindowsInputDevice::ysWindowsInputDevice(InputDeviceType inputDeviceType) : ysInputDevice(Platform::Windows, inputDeviceType) {
    m_deviceHandle = NULL;
    memset(&m_info, 0, sizeof(m_info));
    m_systemName[0] = 0;
}

ysWindowsInputDevice::~ysWindowsInputDevice() {
    /* void */
}

void ysWindowsInputDevice::MakeGeneric() {
    /* void */
}