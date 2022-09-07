#include "../include/yds_sdl_audio_device.h"

#include "../include/yds_sdl_audio_buffer.h"
#include "../include/yds_sdl_audio_source.h"

ysSdlAudioDevice::ysSdlAudioDevice() : ysAudioDevice(API::Sdl) {
    m_deviceId = 0;
}

ysSdlAudioDevice::~ysSdlAudioDevice() {
    /* void */
}

ysAudioBuffer *ysSdlAudioDevice::CreateBuffer(const ysAudioParameters *parameters, SampleOffset size) {
    ysSdlAudioBuffer *newBuffer = m_audioBuffers.NewGeneric<ysSdlAudioBuffer>();
    newBuffer->Initialize(size, *parameters);

    return newBuffer;
}

ysAudioSource *ysSdlAudioDevice::CreateSource(const ysAudioParameters *parameters, SampleOffset size) {
    // For now we can assume they'll always match the device
    // TODO: convert audio using SDL_ConvertAudio() or SDL_AudioStream.
    if (parameters->m_sampleRate != m_outputFormat.freq ||
        parameters->m_channelCount != m_outputFormat.channels ||
        parameters->m_bitsPerSample != SDL_AUDIO_BITSIZE(m_outputFormat.format)) {
        return nullptr;
    }

    ysSdlAudioSource *newSource = m_audioSources.NewGeneric<ysSdlAudioSource>();

    newSource->m_bufferSize = size;
    newSource->m_audioParameters = *parameters;
    newSource->m_safeWriteFrames = m_outputFormat.size /
        (m_outputFormat.channels * SDL_AUDIO_BITSIZE(m_outputFormat.format) / 8);

    return newSource;
}

ysAudioSource *ysSdlAudioDevice::CreateSource(ysAudioBuffer *sourceBuffer) {
    ysAudioSource *newSource = CreateSource(sourceBuffer->GetAudioParameters(), sourceBuffer->GetSampleCount());
    newSource->SetDataBuffer(sourceBuffer);

    return newSource;
}

void ysSdlAudioDevice::UpdateAudioSources() {
    const int sourceCount = m_audioSources.GetNumObjects();
    if (sourceCount > 0) {
        // Unpause the device if we now have sources
        SDL_PauseAudioDevice(m_deviceId, 0);
    } else {
        // Stop and clear the queue when nothing's left
        SDL_PauseAudioDevice(m_deviceId, 1);
    }
}

void ysSdlAudioDevice::FillBuffer(int16_t *audio, int frames) {
    // Clear the buffer
    memset(audio, 0, frames * sizeof(int16_t));

    // Mix each source in
    const int sourceCount = m_audioSources.GetNumObjects();
    for (int i = 0; i < sourceCount; ++i) {
        auto *sdlBuffer = static_cast<ysSdlAudioSource *>(m_audioSources.Get(i));
        sdlBuffer->AddToBuffer(audio, frames);
    }
}
