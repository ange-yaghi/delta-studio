#include "../include/yds_ds8_system.h"

#include "../include/yds_ds8_device.h"
#include "../include/yds_windows_window.h"

ysDS8System::ysDS8System() : ysAudioSystem(API::DirectSound8) {
    /* void */
}

ysDS8System::~ysDS8System() {
    /* void */
}

void ysDS8System::EnumerateDevices() {
    ysAudioSystem::EnumerateDevices();

    if (FAILED(DirectSoundEnumerate((LPDSENUMCALLBACK)DirectSoundEnumProc, (void *)this))) {
        // Could not enumerate devices
        int breakHere=0;
    }
}

void ysDS8System::ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) {
    ysAudioSystem::ConnectDevice(device, windowAssociation);

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    HRESULT result;

    result = DirectSoundCreate8(&ds8Device->m_guid, &ds8Device->m_device, NULL);

    if (FAILED(result)) {
        // Error message
        int breakHere=0;
    }

    //RaiseError(windowAssociation->GetPlatform() == ysWindow::Platform::Windows, "Incompatible platform for audio device window association.");
    ysWindowsWindow *windowsWindow = static_cast<ysWindowsWindow *>(windowAssociation);

    result = ds8Device->m_device->SetCooperativeLevel(windowsWindow->GetWindowHandle(), DSSCL_PRIORITY);

    if (FAILED(result)) {
        // Error message
        int breakHere=0;
    }
}

void ysDS8System::ConnectDeviceConsole(ysAudioDevice *device) {
    ysAudioSystem::ConnectDeviceConsole(device);

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    HRESULT result;

    result = DirectSoundCreate8(&ds8Device->m_guid, &ds8Device->m_device, NULL);

    if (FAILED(result)) {
        // Error message
        int a = 0;
    }

    result = ds8Device->m_device->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);

    if (FAILED(result)) {
        // Error message
        int a = 0;
    }
}

void ysDS8System::DisconnectDevice(ysAudioDevice *device) {
    ysAudioSystem::DisconnectDevice(device);

    ysDS8Device *ds8Device = static_cast<ysDS8Device *>(device);
    ds8Device->m_device->Release();
}

ysDS8Device *ysDS8System::AddDS8Device() {
    return m_devices.NewGeneric<ysDS8Device>();
}

BOOL CALLBACK ysDS8System::DirectSoundEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext) {
    ysDS8System *system = static_cast<ysDS8System *>(lpContext);

    GUID actual;
    GetDeviceID(&DSDEVID_DefaultPlayback, &actual);

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
