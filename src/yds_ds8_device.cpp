#include "../include/yds_ds8_device.h"

#include "../include/yds_ds8_audio_buffer.h"
#include "../include/yds_ds8_audio_source.h"

ysDS8Device::ysDS8Device() : ysAudioDevice(API::DirectSound8) {
    memset(&m_guid, 0, sizeof(GUID));
    m_device = nullptr;
}

ysDS8Device::~ysDS8Device() {
    /* void */
}

ysError ysDS8Device::CreateBuffer(const ysAudioParameters *parameters, SampleOffset size, ysAudioBuffer **buffer) {
    YDS_ERROR_DECLARE("CreateBuffer");

    *buffer = m_audioBuffers.NewGeneric<ysDS8AudioBuffer>();
    YDS_NESTED_ERROR_CALL((*buffer)->Initialize(size, *parameters));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8Device::CreateSource(const ysAudioParameters *parameters, SampleOffset size, ysAudioSource **source) {
    YDS_ERROR_DECLARE("CreateSource");

    ysDS8AudioSource *newSource = m_audioSources.NewGeneric<ysDS8AudioSource>();
    *source = newSource;

    // --------------------------------------------------------
    // TEMP
    // --------------------------------------------------------
    WAVEFORMATEX wfx; 
    DSBUFFERDESC dsbdesc; 
    //LPDIRECTSOUNDBUFFER pDsb = NULL;
    HRESULT hr; 
 
    // Set up WAV format structure. 

    memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
    wfx.wFormatTag = WAVE_FORMAT_PCM; 
    wfx.nChannels = parameters->m_channelCount; 
    wfx.nSamplesPerSec = parameters->m_sampleRate;
    wfx.nBlockAlign = parameters->m_channelCount * (parameters->m_bitsPerSample / 8); 
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
    wfx.wBitsPerSample = parameters->m_bitsPerSample; 

    newSource->m_bufferSize = size;
    newSource->m_audioParameters = *parameters;
 
    // Set up DSBUFFERDESC structure. 
 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = 
        DSBCAPS_CTRLPAN | 
        DSBCAPS_CTRLVOLUME | 
        DSBCAPS_CTRLFREQUENCY | 
        DSBCAPS_GLOBALFOCUS |
        DSBCAPS_GETCURRENTPOSITION2;
    dsbdesc.dwBufferBytes = parameters->GetSizeFromSamples(size); 
    dsbdesc.lpwfxFormat = &wfx; 
 
    // Create buffer. 
    IDirectSoundBuffer *buffer = nullptr;
    hr = m_device->CreateSoundBuffer(&dsbdesc, &buffer, NULL); 
    if (SUCCEEDED(hr)) { 
        if (!SUCCEEDED(buffer->QueryInterface(
            IID_IDirectSoundBuffer8, (LPVOID *)&newSource->m_buffer))) {
            return YDS_ERROR_RETURN(ysError::CouldNotCreateSoundBuffer);
        }

        buffer->Release();
    } 
    else {
        return YDS_ERROR_RETURN(ysError::CouldNotCreateSoundBuffer);
    }

    // --------------------------------------------------------

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8Device::CreateSource(ysAudioBuffer *sourceBuffer, ysAudioSource **source) {
    YDS_ERROR_DECLARE("CreateSource");

    YDS_NESTED_ERROR_CALL(CreateSource(sourceBuffer->GetAudioParameters(), sourceBuffer->GetSampleCount(), source));
    (*source)->SetDataBuffer(sourceBuffer);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysDS8Device::UpdateAudioSources() {
    const int sourceCount = m_audioSources.GetNumObjects();
    for(int i = sourceCount - 1; i >= 0; --i) {
        ysDS8AudioSource *ds8Buffer = static_cast<ysDS8AudioSource *>(m_audioSources.Get(i));

        if (ds8Buffer->GetBufferMode() == ysAudioSource::Mode::PlayOnce) {
            DWORD status;
            if (ds8Buffer->m_buffer->GetStatus(&status) == TRUE) {
                if ((status & DSBSTATUS_PLAYING) == 0) {
                    ds8Buffer->Destroy();
                    m_audioBuffers.Delete(ds8Buffer->GetIndex());
                }
            }
        }
    }
}
