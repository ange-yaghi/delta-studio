#include "../include/yds_windows_input_system.h"
#include "../include/yds_windows_input_device.h"

#include "../include/yds_key_maps.h"

#include <Windows.h>
//#include <strsafe.h>

ysWindowsInputSystem::ysWindowsInputSystem() : ysInputSystem(Platform::WINDOWS) {
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
    int n = m_inputDeviceArray.GetNumObjects();

    for (int i = 0; i < n; i++) {
        ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(m_inputDeviceArray.Get(i));
        if (windowsDevice->m_deviceHandle == hDevice) return windowsDevice;
    }

    return NULL;
}

ysWindowsInputDevice *ysWindowsInputSystem::SystemNameDeviceLookup(char *systemName) {
    int n = m_inputDeviceArray.GetNumObjects();

    for (int i = 0; i < n; i++) {
        ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(m_inputDeviceArray.Get(i));
        if (strcmp(systemName, windowsDevice->m_systemName) == 0) return windowsDevice;
    }

    return NULL;
}

ysError ysWindowsInputSystem::CheckDeviceStatus(ysInputDevice *device) {
    YDS_ERROR_DECLARE("CheckDeviceStatus");

    if (device == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

    ysWindowsInputDevice *windowsDevice = static_cast<ysWindowsInputDevice *>(device);

    UINT nDevices;
    PRAWINPUTDEVICELIST pRawInputDeviceList;

    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return YDS_ERROR_RETURN(ysError::YDS_NO_DEVICE_LIST);

    pRawInputDeviceList = new RAWINPUTDEVICELIST[nDevices];
    if (pRawInputDeviceList == NULL) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_MEMORY);
    if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) return YDS_ERROR_RETURN(ysError::YDS_NO_DEVICE_LIST);

    for (unsigned int i = 0; i < nDevices; i++) {
        RID_DEVICE_INFO info;
        UINT size = sizeof(info);
        GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &info, &size);

        ysInputDevice::InputDeviceType type = TranslateType(info.dwType);

        if (pRawInputDeviceList[i].hDevice != windowsDevice->m_deviceHandle) continue;
        if (type != device->GetType()) continue;
        return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); // Device exists
    }

    // Device no longer exists
    DisconnectDevice(device);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysWindowsInputSystem::CheckAllDevices() {
    YDS_ERROR_DECLARE("CheckAllDevices");

    // More efficient implementation which only loads the input device list once

    UINT nDevices;
    PRAWINPUTDEVICELIST pRawInputDeviceList;

    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) return YDS_ERROR_RETURN(ysError::YDS_NO_DEVICE_LIST);

    pRawInputDeviceList = new RAWINPUTDEVICELIST[nDevices];
    if (pRawInputDeviceList == NULL) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_MEMORY);
    if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) return YDS_ERROR_RETURN(ysError::YDS_NO_DEVICE_LIST);

    int deviceCount = GetDeviceCount();

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

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysWindowsInputSystem::CreateDevices(bool supportMultiple) {
    YDS_ERROR_DECLARE("CreateDevices");

    YDS_NESTED_ERROR_CALL(ysInputSystem::CreateDevices(supportMultiple));

    RAWINPUTDEVICE deviceList[2];

    deviceList[0].usUsagePage = 0x01;
    deviceList[0].usUsage = 0x02;
    deviceList[0].dwFlags = 0; // RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
    deviceList[0].hwndTarget = 0;

    deviceList[1].usUsagePage = 0x01;
    deviceList[1].usUsage = 0x06;
    deviceList[1].dwFlags = 0; //RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
    deviceList[1].hwndTarget = 0;

    if (RegisterRawInputDevices(deviceList, 2, sizeof(RAWINPUTDEVICE)) == FALSE) {
        return YDS_ERROR_RETURN(ysError::YDS_COULD_NOT_REGISTER_FOR_INPUT);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysInputDevice *ysWindowsInputSystem::CreateDevice(ysInputDevice::InputDeviceType type, int id) {
    const char *baseName;
    char deviceName[ysInputDevice::MAX_NAME_LENGTH];

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
    ysWindowsInputDevice *newDevice = NULL;

    if (nameLookup && !nameLookup->IsConnected()) newDevice = nameLookup;
    else newDevice = static_cast<ysWindowsInputDevice *>(FindGenericSlot(type));

    if (newDevice == NULL) {
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

    if (GetRawInputData(lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) return 0;

    RAWINPUT *raw = (RAWINPUT *)lpb;

    ysInputDevice *device = NULL;

    if (raw->header.hDevice != NULL) {
        if (m_supportMultiple) {
            device = DeviceLookup(raw->header.hDevice);

            if (device == NULL) // NEW DEVICE
                device = AddDevice(raw);
        }
        else device = GetMainDevice(TranslateType(raw->header.dwType));
    }

    if (device) {
        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            ysKeyboard *keyboard = device->GetAsKeyboard();
            ysKeyboard::KEY_CODE index = keyboard->GetKeyMap(raw->data.keyboard.VKey);
            const ysKey *key = keyboard->GetKey(index);

            ysKey::KEY_CONF newConf = key->m_configuration;
            ysKey::KEY_STATE newState = key->m_state;

            if (raw->data.keyboard.Flags & RI_KEY_E0)		newConf = ysKey::KEY_CONF_LEFT;
            else if (raw->data.keyboard.Flags & RI_KEY_E1)	newConf = ysKey::KEY_CONF_RIGHT;

            newState = ysKey::KEY_DOWN_TRANS; // Default
            if (raw->data.keyboard.Flags & RI_KEY_BREAK)	newState = ysKey::KEY_UP_TRANS;

            keyboard->SetKeyState(index, newState, newConf);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE) {
            ysMouse *mouse = device->GetAsMouse();

            bool delta = true;
            if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) delta = false;

            mouse->UpdatePosition(raw->data.mouse.lLastX, raw->data.mouse.lLastY, delta);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) mouse->UpdateWheel((int)((short)raw->data.mouse.usButtonData));

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) mouse->UpdateButton(ysMouse::BUTTON_LEFT, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) mouse->UpdateButton(ysMouse::BUTTON_LEFT, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) mouse->UpdateButton(ysMouse::BUTTON_RIGHT, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) mouse->UpdateButton(ysMouse::BUTTON_RIGHT, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) mouse->UpdateButton(ysMouse::BUTTON_MIDDLE, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) mouse->UpdateButton(ysMouse::BUTTON_MIDDLE, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) mouse->UpdateButton(ysMouse::BUTTON_1, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) mouse->UpdateButton(ysMouse::BUTTON_1, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) mouse->UpdateButton(ysMouse::BUTTON_2, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) mouse->UpdateButton(ysMouse::BUTTON_2, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) mouse->UpdateButton(ysMouse::BUTTON_3, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) mouse->UpdateButton(ysMouse::BUTTON_3, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) mouse->UpdateButton(ysMouse::BUTTON_4, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) mouse->UpdateButton(ysMouse::BUTTON_4, ysKey::KEY_UP_TRANS);

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) mouse->UpdateButton(ysMouse::BUTTON_5, ysKey::KEY_DOWN_TRANS);
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) mouse->UpdateButton(ysMouse::BUTTON_5, ysKey::KEY_UP_TRANS);
        }
    }

    delete[] lpb;
    return 0;
}
