#include "../include/yds_windows_input_device.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

//#include "win32/windows_modular.h"

// TODO: -


#elif defined(_WIN64)

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

#endif /* Windows */
