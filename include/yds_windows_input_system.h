#ifndef YDS_WINDOWS_INPUT_SYSTEM_H
#define YDS_WINDOWS_INPUT_SYSTEM_H

#include "yds_input_system.h"

#define NOMINMAX
#include <Windows.h>

class ysWindowsInputDevice;

class ysWindowsInputSystem : public ysInputSystem {
public:
    ysWindowsInputSystem();
    virtual ~ysWindowsInputSystem();

    int ProcessInputMessage(HRAWINPUT lparam);
    int OnOsKey(LPARAM lParam, WPARAM wParam);
    int OnOsMouseButtonDown(ysMouse::Button button);
    int OnOsMouseButtonUp(ysMouse::Button button);
    int OnOsMouseWheel(LPARAM lParam, WPARAM wParam);
    int OnOsMouseMove(LPARAM lParam, WPARAM wParam);

protected:
    virtual ysError CreateDevices();

    virtual ysError CheckDeviceStatus(ysInputDevice *device);
    virtual ysError CheckAllDevices();

    virtual ysInputDevice *CreateDevice(ysInputDevice::InputDeviceType type, int id) override;
    virtual ysInputDevice *CreateVirtualDevice(ysInputDevice::InputDeviceType type) override;
    ysWindowsInputDevice *AddDevice(RAWINPUT *rawInput);

    ysWindowsInputDevice *DeviceLookup(HANDLE hDevice);
    ysWindowsInputDevice *SystemNameDeviceLookup(char *systemName);

    // Translate a windows device type to INPUT_DEVICE_TYPE
    static ysInputDevice::InputDeviceType TranslateType(int type);
};

#endif /* YDS_WINDOWS_INPUT_SYSTEM_H */
