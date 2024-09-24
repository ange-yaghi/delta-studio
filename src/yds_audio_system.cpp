#include "../include/yds_audio_system.h"

#include "../include/yds_audio_buffer.h"
#include "../include/yds_audio_device.h"
#include "../include/yds_ds8_system.h"

#include <fstream>

ysAudioSystem::ysAudioSystem()
    : ysAudioSystemObject("AUDIO_SYSTEM", API::Undefined) {
    m_primaryDevice = nullptr;
}

ysAudioSystem::ysAudioSystem(API api)
    : ysAudioSystemObject("AUDIO_SYSTEM", api) {
    m_primaryDevice = nullptr;
}

ysAudioSystem::~ysAudioSystem() {}

ysError ysAudioSystem::CreateAudioSystem(ysAudioSystem **newAudioSystem,
                                         API api) {
    YDS_ERROR_DECLARE("CreateAudioSystem");

    if (newAudioSystem == nullptr) {
        return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    }
    *newAudioSystem = nullptr;

    if (api == API::Undefined) {
        return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    }

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

    if (audioSystem == nullptr) {
        return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    }

    delete *audioSystem;
    *audioSystem = nullptr;

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysAudioSystem::EnumerateDevices() {
    YDS_ERROR_DECLARE("EnumerateDevices");

    for (int i = 0; i < m_devices.GetNumObjects(); ++i) {
        m_devices.Get(i)->m_available = false;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysAudioDevice *ysAudioSystem::GetPrimaryDevice() { return m_primaryDevice; }

ysAudioDevice *ysAudioSystem::GetDevice(int device) {
    if (device >= GetDeviceCount()) { return nullptr; }
    return m_devices.Get(device);
}

ysError ysAudioSystem::ConnectDevice(ysAudioDevice *device,
                                     ysWindow *windowAssociation) {
    YDS_ERROR_DECLARE("ConnectDevice");

    device->m_connected = true;
    device->m_windowAssociation = windowAssociation;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::ConnectDevice(ysWindow *windowAssociation,
                                     ysAudioDevice **device) {
    YDS_ERROR_DECLARE("ConnectDevice");

    *device = nullptr;

    if (GetPrimaryDevice() == nullptr) {
        return YDS_ERROR_RETURN(ysError::NoAudioDevice);
    }

    ysError err = ConnectDevice(GetPrimaryDevice(), windowAssociation);
    if (err == ysError::None) {
        *device = GetPrimaryDevice();
        return YDS_ERROR_RETURN(ysError::None);
    }

    return YDS_ERROR_RETURN(ysError::NoAudioDevice);
}

ysError ysAudioSystem::ConnectDeviceConsole(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("ConnectDeviceConsole");

    device->m_connected = true;
    device->m_windowAssociation = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::DisconnectDevice(ysAudioDevice *device) {
    YDS_ERROR_DECLARE("DisconnectDevice");

    if (device != nullptr) {
        device->m_connected = false;
        device->m_windowAssociation = nullptr;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::DestroyAudioBuffers() {
    YDS_ERROR_DECLARE("DestroyAudioBuffers");

    const int count = m_audioBuffers.GetNumObjects();
    for (int i = count - 1; i >= 0; i--) {
        ysAudioBuffer *buffer = m_audioBuffers.Get(i);
        YDS_NESTED_ERROR_CALL(DestroyAudioBuffer(buffer));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::DestroyAudioBuffer(ysAudioBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyAudioBuffer");

    if (buffer == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    buffer->m_destroyed = true;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioSystem::UpdateDeviceStates() {
    YDS_ERROR_DECLARE("UpdateDeviceStates");
    YDS_NESTED_ERROR_CALL(EnumerateDevices());

    for (int i = 0; i < m_devices.GetNumObjects(); ++i) {
        m_devices.Get(i)->Update();
    }

    return YDS_ERROR_RETURN(ysError::None);
}
