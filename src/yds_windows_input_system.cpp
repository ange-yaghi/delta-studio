#include "../include/yds_windows_input_system.h"

#include "../include/yds_windows_input_device.h"
#include "../include/yds_windows_window_system.h"
#include "../include/yds_windows_window.h"
#include "../include/yds_key_maps.h"

#include <Windows.h>

ysWindowsInputSystem::ysWindowsInputSystem() : ysInputSystem(Platform::Windows) {
    /* void */
}

ysWindowsInputSystem::~ysWindowsInputSystem() {
    /* void */
}

ysInputDevice::InputDeviceType ysWindowsInputSystem::TranslateType(int type) {
    switch (type) {
    case RIM_TYPEHID:
        return ysInputDevice::InputDeviceType::CUSTOM;
    case RIM_TYPEKEYBOARD:
        return ysInputDevice::InputDeviceType::KEYBOARD;
    case RIM_TYPEMOUSE:
        return ysInputDevice::InputDeviceType::MOUSE;
    }

    return ysInputDevice::InputDeviceType::UNKNOWN;
}

ysWindowsInputDevice *ysWindowsInputSystem::DeviceLookup(HANDLE hDevice) {
    const int n = m_inputDeviceArray.GetNumObjects();
    for (int i = 0; i < n; i++) {
        ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(m_inputDeviceArray.Get(i));
        if (windowsDevice->m_deviceHandle == hDevice) return windowsDevice;
    }

    return nullptr;
}

ysWindowsInputDevice *ysWindowsInputSystem::SystemNameDeviceLookup(char *systemName) {
    const int n = m_inputDeviceArray.GetNumObjects();
    for (int i = 0; i < n; i++) {
        ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(m_inputDeviceArray.Get(i));
        if (strcmp(systemName, windowsDevice->m_systemName) == 0) return windowsDevice;
    }

    return nullptr;
}

ysError ysWindowsInputSystem::CheckDeviceStatus(ysInputDevice *device) {
    YDS_ERROR_DECLARE("CheckDeviceStatus");

    if (device == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(device);

    UINT nDevices;
    PRAWINPUTDEVICELIST pRawInputDeviceList;

    if (GetRawInputDeviceList(nullptr, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return YDS_ERROR_RETURN(ysError::NoDeviceList);

    pRawInputDeviceList = new RAWINPUTDEVICELIST[nDevices];
    if (pRawInputDeviceList == NULL) return YDS_ERROR_RETURN(ysError::OutOfMemory);
    if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) return YDS_ERROR_RETURN(ysError::NoDeviceList);

    for (unsigned int i = 0; i < nDevices; i++) {
        RID_DEVICE_INFO info;
        UINT size = sizeof(info);
        GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &info, &size);

        ysInputDevice::InputDeviceType type = TranslateType(info.dwType);

        if (pRawInputDeviceList[i].hDevice != windowsDevice->m_deviceHandle) continue;
        if (type != device->GetType()) continue;
        return YDS_ERROR_RETURN(ysError::None); // Device exists
    }

    // Device no longer exists
    DisconnectDevice(device);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysWindowsInputSystem::CheckAllDevices() {
    YDS_ERROR_DECLARE("CheckAllDevices");

    // More efficient implementation which only loads the input device list once

    UINT nDevices;
    PRAWINPUTDEVICELIST pRawInputDeviceList;

    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return YDS_ERROR_RETURN(ysError::NoDeviceList);

    pRawInputDeviceList = new RAWINPUTDEVICELIST[nDevices];
    if (pRawInputDeviceList == nullptr) return YDS_ERROR_RETURN(ysError::OutOfMemory);
    if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) return YDS_ERROR_RETURN(ysError::NoDeviceList);

    const int deviceCount = GetDeviceCount();

    for (int j = deviceCount - 1; j >= 0; j--) {
        ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(m_inputDeviceArray.Get(j));

        for (unsigned int i = 0; i < nDevices; i++) {
            RID_DEVICE_INFO info;
            UINT size = sizeof(info);
            GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &info, &size);

            ysInputDevice::InputDeviceType type = TranslateType(info.dwType);

            if (pRawInputDeviceList[i].hDevice != windowsDevice->m_deviceHandle) continue;
            if (type != windowsDevice->GetType()) continue;
            break; // Device exists
        }

        // Device no longer exists
        DisconnectDevice(windowsDevice);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysWindowsInputSystem::CreateDevices() {
    YDS_ERROR_DECLARE("CreateDevices");

    RAWINPUTDEVICE deviceList[2];
    
    ysWindowsWindow *primaryWindow = m_windowSystem->GetWindowCount() > 0
        ? static_cast<ysWindowsWindow *>(m_windowSystem->GetWindow(0))
        : nullptr;

    deviceList[0].usUsagePage = 0x01;
    deviceList[0].usUsage = 0x02;
    deviceList[0].dwFlags = RIDEV_INPUTSINK;
    deviceList[0].hwndTarget = primaryWindow != nullptr
        ? primaryWindow->GetWindowHandle()
        : NULL;

    deviceList[1].usUsagePage = 0x01;
    deviceList[1].usUsage = 0x06;
    deviceList[1].dwFlags = RIDEV_INPUTSINK;
    deviceList[1].hwndTarget = primaryWindow != nullptr
        ? primaryWindow->GetWindowHandle()
        : NULL;

    if (RegisterRawInputDevices(deviceList, 2, sizeof(RAWINPUTDEVICE)) == FALSE) {
        return YDS_ERROR_RETURN(ysError::CouldNotRegisterForInput);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysInputDevice *ysWindowsInputSystem::CreateDevice(ysInputDevice::InputDeviceType type, int id) {
    const char *baseName;
    char deviceName[128];

    switch (type) {
    case ysInputDevice::InputDeviceType::CUSTOM:
        baseName = "HID";
        break;
    case ysInputDevice::InputDeviceType::KEYBOARD:
        baseName = "KEYBOARD";
        break;
    case ysInputDevice::InputDeviceType::MOUSE:
        baseName = "MOUSE";
        break;
    default:
        baseName = "";
    }

    int deviceID;
    if (id >= 0) deviceID = id;
    else deviceID = GetNextDeviceID(type);

    ysWindowsInputDevice *newDevice = m_inputDeviceArray.NewGeneric<ysWindowsInputDevice>();
    sprintf_s(deviceName, 256, "%s%0.3d", baseName, deviceID);

    newDevice->SetName(deviceName);
    newDevice->SetType(type);
    newDevice->SetDeviceID(deviceID);
    newDevice->m_deviceHandle = NULL;
    newDevice->SetGeneric(true);

    memset(&newDevice->m_info, 0, sizeof(newDevice->m_info));

    if (type == ysInputDevice::InputDeviceType::KEYBOARD) {
        // Load Keymap
        ysKeyboard *keyboard = newDevice->GetAsKeyboard();
        keyboard->RegisterKeyMap(ysKeyMaps::GetWindowsKeyMap());
    }

    RegisterDevice(newDevice);

    return newDevice;
}

ysWindowsInputDevice *ysWindowsInputSystem::AddDevice(RAWINPUT *rawInput) {
    // Check whether a device exists already

    RID_DEVICE_INFO info;
    UINT size = sizeof(info);

    UINT nameSize = 256;
    char systemName[256];

    ysInputDevice::InputDeviceType type = TranslateType(rawInput->header.dwType);

    GetRawInputDeviceInfo(rawInput->header.hDevice, RIDI_DEVICEINFO, &info, &size);
    GetRawInputDeviceInfo(rawInput->header.hDevice, RIDI_DEVICENAME, systemName, &nameSize);

    ysWindowsInputDevice *nameLookup = SystemNameDeviceLookup(systemName);
    ysWindowsInputDevice *newDevice = nullptr;

    if (nameLookup && !nameLookup->IsConnected()) newDevice = nameLookup;
    else newDevice = static_cast<ysWindowsInputDevice *>(FindGenericSlot(type));

    if (newDevice == nullptr) {
        newDevice = static_cast<ysWindowsInputDevice *>(CreateDevice(type, -1));
    }

    newDevice->m_deviceHandle = rawInput->header.hDevice;
    newDevice->m_info = info;
    newDevice->SetGeneric(false);
    newDevice->SetConnected(true);
    strcpy_s(newDevice->m_systemName, 256, systemName);

    return newDevice;
}

int ysWindowsInputSystem::ProcessInputMessage(HRAWINPUT lparam) {
    UINT dwSize;
    LPBYTE lpb;

    GetRawInputData(lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

    lpb = new BYTE[dwSize];
    if (lpb == nullptr) return 0;

    if (GetRawInputData(lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
        delete[] lpb;
        return 0;
    }

    RAWINPUT *raw = (RAWINPUT *)lpb;

    ysInputDevice *device = nullptr;

    if (raw->header.hDevice != nullptr) {
        device = DeviceLookup(raw->header.hDevice);

        if (device == nullptr) {
            device = AddDevice(raw);
        }
    }

    if (device != nullptr) {
        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            ysKeyboard *keyboard = device->GetAsKeyboard();
            ysKey::Code index = keyboard->GetKeyMap(raw->data.keyboard.VKey);
            const ysKey *key = keyboard->GetKey(index);

            ysKey::Variation newConf = key->m_configuration;
            ysKey::State newState = key->m_state;

            if (raw->data.keyboard.Flags & RI_KEY_E0) newConf = ysKey::Variation::Left;
            else if (raw->data.keyboard.Flags & RI_KEY_E1) newConf = ysKey::Variation::Right;

            newState = ysKey::State::DownTransition; // Default
            if (raw->data.keyboard.Flags & RI_KEY_BREAK) newState = ysKey::State::UpTransition;

            keyboard->SetKeyState(index, newState, newConf);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE) {
            ysMouse *mouse = device->GetAsMouse();

            bool delta = true;
            if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) delta = false;

            POINT p;
            if (GetCursorPos(&p)) {
                mouse->SetOsPosition(p.x, p.y);
            }

            mouse->UpdatePosition(raw->data.mouse.lLastX, raw->data.mouse.lLastY, delta);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) mouse->UpdateWheel((int)((short)raw->data.mouse.usButtonData));

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) mouse->UpdateButton(ysMouse::Button::Left, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) mouse->UpdateButton(ysMouse::Button::Left, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) mouse->UpdateButton(ysMouse::Button::Right, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) mouse->UpdateButton(ysMouse::Button::Right, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) mouse->UpdateButton(ysMouse::Button::Middle, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) mouse->UpdateButton(ysMouse::Button::Middle, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) mouse->UpdateButton(ysMouse::Button::Aux_1, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) mouse->UpdateButton(ysMouse::Button::Aux_1, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) mouse->UpdateButton(ysMouse::Button::Aux_2, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) mouse->UpdateButton(ysMouse::Button::Aux_2, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) mouse->UpdateButton(ysMouse::Button::Aux_3, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) mouse->UpdateButton(ysMouse::Button::Aux_3, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) mouse->UpdateButton(ysMouse::Button::Aux_4, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) mouse->UpdateButton(ysMouse::Button::Aux_4, ysMouse::ButtonState::UpTransition);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) mouse->UpdateButton(ysMouse::Button::Aux_5, ysMouse::ButtonState::DownTransition);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) mouse->UpdateButton(ysMouse::Button::Aux_5, ysMouse::ButtonState::UpTransition);
        }
    }

    delete[] lpb;
    return 0;
}
