#include "../include/yds_audio_streaming.h"

#include "../include/yds_audio_source.h"
#include "../include/yds_audio_file.h"

ysStreamingAudio::ysStreamingAudio() : ysObject("ysStreamingAudio") {
    m_file = nullptr;
    m_source = nullptr;
    m_fileOffset = 0;

    m_subdivisions = 0;
    m_currentWriteSubdivision = 0;
    m_currentReadSubdivision = 0;
}

ysStreamingAudio::~ysStreamingAudio() {
    /* void */
}

ysError ysStreamingAudio::AttachAudioFile(ysAudioFile *file) {
    YDS_ERROR_DECLARE("AttachAudioFile");

    ysError error;
    m_file = file;

    error = UpdateMetrics();
    if (error != ysError::None) m_file = nullptr;

    return YDS_ERROR_RETURN(error);
}

ysError ysStreamingAudio::AttachAudioSource(ysAudioSource *source) {
    YDS_ERROR_DECLARE("AttachAudioBuffer");

    ysError error;
    m_source = source;

    error = UpdateMetrics();
    if (error != ysError::None) m_source = nullptr;

    return error;
}

ysError ysStreamingAudio::UpdateMetrics() {
    YDS_ERROR_DECLARE("UpdateMetrics");

    if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::None);
    if (m_source == nullptr) return YDS_ERROR_RETURN(ysError::None);
    if (m_file->GetBufferSize() > m_source->GetBufferSize()) return YDS_ERROR_RETURN(ysError::IncompatibleBufferAndFile);
    if (m_source->GetBufferSize() % m_file->GetBufferSize() > 0) return YDS_ERROR_RETURN(ysError::IncompatibleBufferAndFile);

    m_subdivisions = m_source->GetBufferSize() / m_file->GetBufferSize();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysStreamingAudio::Seek(SampleOffset offset) {
    YDS_ERROR_DECLARE("Seek");

    if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::NoFile);
    if (m_source == nullptr) return YDS_ERROR_RETURN(ysError::NoAudioBuffer);
    if (m_file->GetBufferSize() == 0) return YDS_ERROR_RETURN(ysError::NoFileBuffer);

    ysAudioSource::Mode mode = m_source->GetBufferMode();
    m_source->SetMode(ysAudioSource::Mode::Stop);
    m_source->Seek(0);

    m_fileOffset = offset;
    ysError error = InitializeBuffer();

    m_source->SetMode(mode);

    return YDS_ERROR_RETURN(error);
}

ysError ysStreamingAudio::InitializeBuffer() {
    YDS_ERROR_DECLARE("InitializeBuffer");

    if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::NoFile);
    if (m_source == nullptr) return YDS_ERROR_RETURN(ysError::NoAudioBuffer);
    if (m_file->GetBufferSize() == 0) return YDS_ERROR_RETURN(ysError::NoFileBuffer);

    void *audioBuffer;
    SampleOffset audioBufferSize;
    m_source->LockEntireBuffer(&audioBuffer, &audioBufferSize);

    unsigned int bufferOffset;

    for(int i = 0; i < m_subdivisions; i++) {
        bufferOffset = m_source->GetAudioParameters()->GetSizeFromSamples(i * m_file->GetBufferSize());

        m_file->FillBuffer(m_fileOffset);
        const void *fileBuffer = m_file->GetBuffer();

        memcpy(((char *)audioBuffer) + bufferOffset, fileBuffer, m_file->GetAudioParameters()->GetSizeFromSamples(m_file->GetBufferSize()));
        
        m_fileOffset += m_file->GetBufferSize();
    }

    m_source->UnlockBuffer(audioBuffer, audioBufferSize);

    m_currentWriteSubdivision = 0;
    m_currentReadSubdivision = 0;

    return YDS_ERROR_RETURN(ysError::None);
}

void ysStreamingAudio::Transfer() {
    m_file->FillBuffer(m_fileOffset);

    SampleOffset writeOffset = m_currentWriteSubdivision * m_file->GetBufferSize();

    void *segment1;
    SampleOffset size1;

    void *segment2;
    SampleOffset size2;

    const void *fileBuffer = m_file->GetBuffer();

    ysError error = m_source->LockBufferSegment(
        writeOffset, m_file->GetBufferSize(), &segment1, &size1, &segment2, &size2);

    SampleOffset writeCursor = m_source->GetCurrentWritePosition();

    if (segment1 != nullptr) {
        unsigned int size = m_source->GetAudioParameters()->GetSizeFromSamples(size1);
        memcpy(segment1, fileBuffer, size);
    }

    if (segment2 != nullptr) {
        memcpy(segment2, 
            ((char *)fileBuffer) + m_source->GetAudioParameters()->GetSizeFromSamples(size1), 
            m_source->GetAudioParameters()->GetSizeFromSamples(size2));
    }

    m_source->UnlockBufferSegments(segment1, size1, segment2, size2);
}

ysError ysStreamingAudio::Update() {
    YDS_ERROR_DECLARE("Update");

    int readSubdivision;
    SampleOffset readPosition;

    readPosition = m_source->GetCurrentPosition();
    readSubdivision = readPosition / m_file->GetBufferSize();

    if (readSubdivision != m_currentReadSubdivision) {
        if (readSubdivision != ((m_currentReadSubdivision + 1) % m_subdivisions)) {
            // TEMP
            // Weird jump, don't know how to handle this yet
        }

        m_currentReadSubdivision = readSubdivision;

        Transfer();
        m_currentWriteSubdivision += 1;
        m_currentWriteSubdivision %= m_subdivisions;

        m_fileOffset += m_file->GetBufferSize();
    }

    return YDS_ERROR_RETURN(ysError::None);
}
