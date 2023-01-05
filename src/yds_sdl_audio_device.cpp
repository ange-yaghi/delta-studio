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
        SDL_ClearQueuedAudio(m_deviceId);
    }

    // TODO: we currently assume 1 source, otherwise we'd need to mix them here
    for (int i = 0; i < sourceCount; ++i) {
        auto *sdlBuffer = static_cast<ysSdlAudioSource *>(m_audioSources.Get(i));

        void *data1;
        void *data2;
        size_t length1;
        size_t length2;

        // Try and take the lock on this buffer
        if (sdlBuffer->TryLockForRead(&data1, &length1, &data2, &length2)) {
            // Submit the audio
            SDL_QueueAudio(m_deviceId, data1, length1);
            SDL_QueueAudio(m_deviceId, data2, length2);

            // We're done with the buffer
            sdlBuffer->UnlockForRead();
        }
    }
}
