#include "../include/yds_audio_device.h"

#include "../include/yds_audio_buffer.h"
#include "../include/yds_audio_source.h"

ysAudioDevice::ysAudioDevice() : ysAudioSystemObject("AUDIO_DEVICE", API::Undefined) {
    m_deviceName[0] = '\0';
    m_connected = false;
    m_windowAssociation = 0;
}

ysAudioDevice::ysAudioDevice(API api) : ysAudioSystemObject("ysAudioDevice", api) {
    m_deviceName[0] = '\0';
    m_connected = false;
    m_windowAssociation = 0;
}

ysAudioDevice::~ysAudioDevice() {
    /* void */
}

ysError ysAudioDevice::DestroyAudioBuffer(ysAudioBuffer *&buffer) {
    YDS_ERROR_DECLARE("DestroyaAudioBuffer");

    if (buffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    buffer->Destroy();

    m_audioBuffers.Delete(buffer->GetIndex());
    buffer = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioDevice::DestroyAudioSource(ysAudioSource *&source) {
    YDS_ERROR_DECLARE("DestroyAudioSource");

    if (source == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    source->Destroy();

    m_audioBuffers.Delete(source->GetIndex());
    source = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAudioDevice::DestroyAudioBuffers() {
    YDS_ERROR_DECLARE("DestroyAudioBuffers");

    int count = m_audioBuffers.GetNumObjects();

    for(int i = count - 1; i >= 0; i--) {
        ysAudioBuffer *buffer = m_audioBuffers.Get(i);
        YDS_NESTED_ERROR_CALL(DestroyAudioBuffer(buffer));
    }

    return YDS_ERROR_RETURN(ysError::None);
}
