#ifndef YDS_WINDOWS_INPUT_DEVICE_H
#define YDS_WINDOWS_INPUT_DEVICE_H

#include "yds_input_device.h"

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/window.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif

class ysWindowsInputDevice : public ysInputDevice {
    friend class ysWindowsInputSystem;

public:
    ysWindowsInputDevice();
    ysWindowsInputDevice(InputDeviceType type);
    ~ysWindowsInputDevice();

    void MakeGeneric();

protected:
    HANDLE m_deviceHandle;
    RID_DEVICE_INFO m_info;
    wchar_t m_systemName[256];
};

#endif /* YDS_WINDOWS_INPUT_DEVICE_H */
