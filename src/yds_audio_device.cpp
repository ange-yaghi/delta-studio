#include "../include/yds_audio_device.h"

#include "../include/yds_audio_buffer.h"
#include "../include/yds_audio_source.h"

ysAudioDevice::ysAudioDevice()
    : ysAudioSystemObject("AUDIO_DEVICE", API::Undefined) {
    m_deviceName[0] = '\0';
    m_connected = false;
    m_available = false;
    m_windowAssociation = 0;
}

ysAudioDevice::ysAudioDevice(API api)
    : ysAudioSystemObject("ysAudioDevice", api) {
    m_deviceName[0] = '\0';
    m_connected = false;
    m_available = false;
    m_windowAssociation = 0;
}

ysAudioDevice::~ysAudioDevice() {}

ysError ysAudioDevice::DestroyAudioBuffer(ysAudioBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyAudioBuffer");

    if (buffer == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    buffer->m_destroyed = true;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioDevice::DestroyAudioSource(ysAudioSource *&source) {
    YDS_ERROR_DECLARE("DestroyAudioSource");

    if (source == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    source->m_destroyed = true;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioDevice::DestroyAudioBuffers() {
    YDS_ERROR_DECLARE("DestroyAudioBuffers");

    const int count = m_audioBuffers.GetNumObjects();
    for (int i = count - 1; i >= 0; i--) {
        ysAudioBuffer *buffer = m_audioBuffers.Get(i);
        YDS_NESTED_ERROR_CALL(DestroyAudioBuffer(buffer));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void ysAudioDevice::Update() {
    for (int i = 0; i < m_audioSources.GetNumObjects(); ++i) {
        ysAudioSource *source = m_audioSources.Get(i);
        if (source->m_destroyed && source->m_released) {
            source->Destroy();
            m_audioSources.Delete(i);
            --i;
        }
    }

    for (int i = 0; i < m_audioBuffers.GetNumObjects(); ++i) {
        ysAudioBuffer *buffer = m_audioBuffers.Get(i);
        if (buffer->m_destroyed && buffer->m_released) {
            buffer->Destroy();
            m_audioBuffers.Delete(i);
            --i;
        }
    }
}
