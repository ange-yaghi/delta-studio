#include "../include/yds_sdl_audio_wave_file.h"

#include <SDL.h>

#include <cstring>

ysSdlAudioWaveFile::ysSdlAudioWaveFile() : ysAudioFile(Platform::Sdl, AudioFormat::Wave) {
    m_dataLength = 0;
    m_data = nullptr;
}

ysSdlAudioWaveFile::~ysSdlAudioWaveFile() {
    DestroyInternalBuffer();
    CloseFile();
}

ysAudioFile::Error ysSdlAudioWaveFile::OpenFile(const char *fname) {
    ysAudioFile::Error error = ysAudioFile::OpenFile(fname);
    if (error != Error::None) return error;

    // Lazy approach: load everything since files are small
    SDL_AudioSpec spec{};
    if (SDL_LoadWAV(fname, &spec, &m_data, &m_dataLength) == nullptr) {
        return Error::CouldNotOpenFile;
    }

    // Everything worked, commit data
    m_audioParameters.m_channelCount = spec.channels;
    m_audioParameters.m_sampleRate = spec.freq;
    m_audioParameters.m_bitsPerSample = SDL_AUDIO_BITSIZE(spec.format);
    m_sampleCount = spec.samples;
    m_fileOpen = true;

    return Error::None;
}

ysAudioFile::Error ysSdlAudioWaveFile::CloseFile() {
    Error error = ysAudioFile::CloseFile();
    if (error != Error::None) return error;

    if (m_data != nullptr) {
        SDL_free(m_data);
        m_data = nullptr;
        m_dataLength = 0;
    }

    return Error::None;
}

ysAudioFile::Error ysSdlAudioWaveFile::GenericRead(SampleOffset offset, SampleOffset size, void *buffer) {
    Error error = ysAudioFile::GenericRead(offset, size, buffer);
    if (error != Error::None) return error;

    // Work out where we need to read from
    const long fileOffset = m_audioParameters.GetSizeFromSamples(offset);
    const long fileSize = m_audioParameters.GetSizeFromSamples(size);

    // Check that there's space
    if (fileSize + fileOffset > m_dataLength) {
        return Error::FileReadError;
    }

    // Read it
    memcpy(buffer, m_data + fileOffset, fileSize);
    return Error::None;
}
