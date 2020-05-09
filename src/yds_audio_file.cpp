#include "../include/yds_audio_file.h"

#include "../include/yds_audio_buffer.h"

ysAudioFile::ysAudioFile() {
    m_format = AudioFormat::Undefined;

    m_sampleCount = 0;
    m_externalBuffer = 0;

    m_maxBufferSamples = 0;
    m_bufferDataSamples = 0;
    m_buffer = nullptr;
    m_currentReadingOffset = 0;

    m_fileOpen = false;
}

ysAudioFile::ysAudioFile(Platform platform, AudioFormat format) : ysWindowSystemObject("AUDIO_FILE", platform) {
    m_format = format;

    m_sampleCount = 0;
    m_externalBuffer = 0;

    m_maxBufferSamples = 0;
    m_bufferDataSamples = 0;
    m_buffer = nullptr;
    m_currentReadingOffset = 0;

    m_fileOpen = false;
}

ysAudioFile::~ysAudioFile() {
    if (m_buffer) delete[] m_buffer;
}

ysAudioFile::Error ysAudioFile::OpenFile(const char *fname) {
    if (m_fileOpen) return Error::FileAlreadyOpen;

    return Error::None;
}

ysAudioFile::Error ysAudioFile::CloseFile() {
    if (!m_fileOpen) return Error::NoFileOpen;

    return Error::None;
}

ysAudioFile::Error ysAudioFile::FillBuffer(SampleOffset offset) {
    if (offset > m_sampleCount) return Error::ReadOutOfRange;

    // Use External Buffer
    if (m_externalBuffer != nullptr) {
        if (offset + m_externalBuffer->GetSampleCount() > m_sampleCount) {
            return Error::ReadOutOfRange;
        }

        void *target = malloc(m_externalBuffer->GetBufferSize());

        Error readError = GenericRead(offset, m_externalBuffer->GetSampleCount(), target);
        if (readError != Error::None) {
            return readError;
        }

        m_externalBuffer->EditBuffer(target);
        free(target);
    }
    // Use Internal Buffer
    else {
        if (offset + m_maxBufferSamples > m_sampleCount) {
            return Error::ReadOutOfRange;
        }

        Error readError = GenericRead(offset, m_maxBufferSamples, (void *)m_buffer);
        if (readError != Error::None) {
            return readError;
        }
    }

    return Error::None;
}

ysAudioFile::Error ysAudioFile::GenericRead(SampleOffset offset, SampleOffset size, void *target) {
    if (!m_fileOpen) return Error::NoFileOpen;

    return Error::None;
}

ysAudioFile::Error ysAudioFile::InitializeInternalBuffer(SampleOffset samples, bool saveData) {
    if (!m_fileOpen) return Error::NoFileOpen;

    int newSize = m_audioParameters.GetSizeFromSamples(samples);
    char *newBuffer = new char[newSize];
    if (newBuffer == nullptr) return Error::OutOfMemory;

    if (saveData && m_buffer != nullptr) {
        int copySamples = (m_bufferDataSamples < samples) ? m_bufferDataSamples : samples;
        int copySize = m_audioParameters.GetSizeFromSamples(copySamples);

        if (copySize > 0) memcpy(newBuffer, m_buffer, copySize);
    }

    // Delete original buffer
    delete[] m_buffer;

    m_buffer = newBuffer;
    m_maxBufferSamples = samples;
    m_bufferDataSamples = 0;
    m_externalBuffer = NULL;

    return Error::None;
}

void ysAudioFile::DestroyInternalBuffer() {
    delete[] m_buffer;

    m_maxBufferSamples = 0;
    m_bufferDataSamples = 0;
    m_buffer = 0;
}

ysAudioFile::Error ysAudioFile::AttachExternalBuffer(ysAudioBuffer *buffer) {
    DestroyInternalBuffer();

    m_externalBuffer = buffer;

    return Error::None;
}
