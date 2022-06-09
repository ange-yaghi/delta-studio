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

ysAudioBuffer *ysDS8Device::CreateBuffer(const ysAudioParameters *parameters, SampleOffset size) {
    ysDS8AudioBuffer *newBuffer = m_audioBuffers.NewGeneric<ysDS8AudioBuffer>();
    newBuffer->Initialize(size, *parameters);

    return newBuffer;
}

ysAudioSource *ysDS8Device::CreateSource(const ysAudioParameters *parameters, SampleOffset size) {
    ysDS8AudioSource *newSource = m_audioSources.NewGeneric<ysDS8AudioSource>();

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
 
    hr = m_device->CreateSoundBuffer(&dsbdesc, &newSource->m_buffer, NULL); 
    if (SUCCEEDED(hr)) { 
        hr = newSource->m_buffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&newSource->m_buffer);
        newSource->m_buffer->Release();
    } 
    else {
        /* void */
    }

    // --------------------------------------------------------

    return newSource;
}

ysAudioSource *ysDS8Device::CreateSource(ysAudioBuffer *sourceBuffer) {
    ysAudioSource *newSource = CreateSource(sourceBuffer->GetAudioParameters(), sourceBuffer->GetSampleCount());
    newSource->SetDataBuffer(sourceBuffer);

    return newSource;
}

void ysDS8Device::UpdateAudioSources() {
    int sourceCount = m_audioSources.GetNumObjects();
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
