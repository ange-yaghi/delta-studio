#include "../include/yds_ds8_system.h"

#include "../include/yds_ds8_device.h"
#include "../include/yds_windows_window.h"

ysDS8System::ysDS8System() : ysAudioSystem(API::DirectSound8) {
    /* void */
}

ysDS8System::~ysDS8System() {
    /* void */
}

ysError ysDS8System::EnumerateDevices() {
    YDS_ERROR_DECLARE("EnumerateDevices");

    YDS_NESTED_ERROR_CALL(ysAudioSystem::EnumerateDevices());

    if (FAILED(DirectSoundEnumerate((LPDSENUMCALLBACK)DirectSoundEnumProc, (void *)this))) {
        return YDS_ERROR_RETURN(ysError::CouldNotEnumerateAudioDevices);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8System::ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) {
    YDS_ERROR_DECLARE("ConnectDevice");

    YDS_NESTED_ERROR_CALL(ysAudioSystem::ConnectDevice(device, windowAssociation));

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    HRESULT result;

    result = DirectSoundCreate8(&ds8Device->m_guid, &ds8Device->m_device, NULL);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDS8Device);
    }

    //RaiseError(windowAssociation->GetPlatform() == ysWindow::Platform::Windows, "Incompatible platform for audio device window association.");
    
    if (windowAssociation != nullptr) {
        ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(windowAssociation);
        result = ds8Device->m_device->SetCooperativeLevel(windowsWindow->GetWindowHandle(), DSSCL_PRIORITY);
    }
    else {
        HWND hWnd = GetForegroundWindow();
        if (hWnd == NULL) {
          hWnd = GetDesktopWindow();
        }

        result = ds8Device->m_device->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
    }

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotSetDeviceCooperativeLevel);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8System::ConnectDeviceConsole(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("ConnectDeviceConsole");

    YDS_NESTED_ERROR_CALL(ysAudioSystem::ConnectDeviceConsole(device));

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    HRESULT result;

    result = DirectSoundCreate8(&ds8Device->m_guid, &ds8Device->m_device, NULL);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateDS8Device);
    }

    result = ds8Device->m_device->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);
    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::CouldNotSetDeviceCooperativeLevel);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8System::DisconnectDevice(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("DisconnectDevice");

    YDS_NESTED_ERROR_CALL(ysAudioSystem::DisconnectDevice(device));

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    ds8Device->m_device->Release();

    return YDS_ERROR_RETURN(ysError::None);
}

ysDS8Device *ysDS8System::AddDS8Device() {
    return m_devices.NewGeneric<ysDS8Device>();
}

BOOL CALLBACK ysDS8System::DirectSoundEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext) {
    ysDS8System *system = static_cast<ysDS8System *>(lpContext);

    GUID actual;
    if (FAILED(GetDeviceID(&DSDEVID_DefaultPlayback, &actual))) {
        return FALSE;
    }

    if (lpGUID != nullptr) {
        if (actual == *lpGUID) {
            ysAudioDevice *primary = system->GetPrimaryDevice();
            primary->SetDeviceName(lpszDesc);
        }
        else {
            ysDS8Device *newDevice = system->AddDS8Device();
            newDevice->m_guid = *lpGUID;
        }
    }
    else {
        // Create blank device
        ysDS8Device *newDevice = system->AddDS8Device();
    }

    return TRUE;
}
