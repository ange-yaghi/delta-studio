#include "../include/yds_audio_system.h"

#include "../include/yds_audio_device.h"
#include "../include/yds_ds8_system.h"

ysAudioSystem::ysAudioSystem() : ysAudioSystemObject("AUDIO_SYSTEM", API::Undefined) {
    /* void */
}

ysAudioSystem::ysAudioSystem(API api) : ysAudioSystemObject("AUDIO_SYSTEM", api) {
    /* void */
}

ysAudioSystem::~ysAudioSystem() {
    /* void */
}

ysError ysAudioSystem::CreateAudioSystem(ysAudioSystem **newAudioSystem, API api) {
    YDS_ERROR_DECLARE("CreateAudioSystem");

    if (newAudioSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    *newAudioSystem = nullptr;

    if (api == API::Undefined) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);

    switch (api) {
    case API::DirectSound8:
        *newAudioSystem = new ysDS8System;
        break;
    default:
        *newAudioSystem = nullptr;
        break;
    }

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysAudioSystem::DestroyAudioSystem(ysAudioSystem **audioSystem) {
    YDS_ERROR_DECLARE("DestroyAudioSystem");

    if (audioSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);

    delete *audioSystem;
    *audioSystem = nullptr;

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysAudioSystem::EnumerateDevices() {
    YDS_ERROR_DECLARE("EnumerateDevices");

    m_devices.Clear();

    return YDS_ERROR_RETURN(ysError::None);
}

ysAudioDevice *ysAudioSystem::GetPrimaryDevice() {
    if (m_devices.GetNumObjects() == 0) { return nullptr; }
    else {
        // Zeroeth device is always the default
        return m_devices.Get(0);
    }
}

ysAudioDevice *ysAudioSystem::GetAuxDevice(int device) {
    if ((device + 1) >= GetDeviceCount()) return 0;
    return m_devices.Get(device + 1);
}

ysError ysAudioSystem::ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) {
    YDS_ERROR_DECLARE("ConnectDevice");

    device->m_connected = true;
    device->m_windowAssociation = windowAssociation;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::ConnectDeviceConsole(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("ConnectDeviceConsole");

    device->m_connected = true;
    device->m_windowAssociation = NULL;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::DisconnectDevice(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("DisconnectDevice");

    device->m_connected = false;
    device->m_windowAssociation = 0;

    return YDS_ERROR_RETURN(ysError::None);
}
