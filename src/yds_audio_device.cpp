#include "yds_audio_device.h"

#include "yds_audio_buffer.h"
#include "yds_audio_source.h"
#include "yds_error_system.h"

ysAudioDevice::ysAudioDevice()
    : ysAudioSystemObject("AUDIO_DEVICE", API::Undefined)
    , m_deviceName({0})
    , m_windowAssociation(nullptr)
    , m_connected(false)
    , m_available(false)
{ }

ysAudioDevice::ysAudioDevice(API api)
    : ysAudioSystemObject("ysAudioDevice", api)
    , m_deviceName({0})
    , m_windowAssociation(nullptr)
    , m_connected(false)
    , m_available(false)
{ }

ysError ysAudioDevice::DestroyAudioSource(ysAudioSource *&source) {
    YDS_ERROR_DECLARE("DestroyAudioSource");

    if (source == nullptr) {
        return YDS_ERROR_RETURN(ysError::InvalidParameter);
    }

    source->m_destroyed = true;

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
}
