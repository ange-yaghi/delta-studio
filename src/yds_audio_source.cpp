#include "../include/yds_audio_source.h"

ysAudioSource::ysAudioSource() : ysAudioSystemObject("AUDIO_BUFFER", API::Undefined) {
    ResetLock();

    m_bufferSize = 0;

    m_bufferMode = Mode::Undefined;

    m_volume = 1.0;
    m_pan = 0.0;
    m_panVolume = 1.0;
}

ysAudioSource::ysAudioSource(API api) : ysAudioSystemObject("AUDIO_BUFFER", api) {
    ResetLock();

    m_bufferSize = 0;

    m_bufferMode = Mode::Undefined;

    m_volume = 1.0;
    m_pan = 0.0;
    m_panVolume = 1.0;
}

ysAudioSource::~ysAudioSource() {
    /* void */
}

ysError ysAudioSource::LockEntireBuffer(void **buffer, SampleOffset *samples) {
    YDS_ERROR_DECLARE("LockEntireBuffer");

    *buffer = nullptr;
    *samples = 0;

    if (m_locked) return YDS_ERROR_RETURN(ysError::YDS_BUFFER_ALREADY_LOCKED);

    m_lockOffset = 0;
    m_lockSegment1Size = m_bufferSize;

    *samples = m_bufferSize;
    m_locked = true;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::UnlockBuffer(void *buffer, SampleOffset samples) {
    YDS_ERROR_DECLARE("UnlockBuffer");

    if (!m_locked) return YDS_ERROR_RETURN(ysError::YDS_BUFFER_NOT_LOCKED);
    if (samples > m_lockSegment1Size) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);

    ResetLock();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::LockBufferSegment(
    SampleOffset offset, SampleOffset samples, 
    void **segment1, SampleOffset *size1, void **segment2, SampleOffset *size2) 
{
    YDS_ERROR_DECLARE("LockBufferSegment");

    if (m_locked) return YDS_ERROR_RETURN(ysError::YDS_BUFFER_ALREADY_LOCKED);
    if (segment1 == nullptr || segment2 == nullptr || samples == 0) {
        return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    }
    if (samples > m_bufferSize) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);
    if (offset > m_bufferSize) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);

    if (offset + samples > m_bufferSize) {
        m_lockSegment2Size = offset + samples - m_bufferSize;
    }
    else m_lockSegment2Size = 0;

    m_lockSegment1Size = samples - m_lockSegment2Size;
    m_locked = true;

    if (size1) *size1 = m_lockSegment1Size;
    if (size2) *size2 = m_lockSegment2Size;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::UnlockBufferSegments(
    void *segment1, SampleOffset segment1Size, 
    void *segment2, SampleOffset segment2Size) 
{
    YDS_ERROR_DECLARE("UnlockBufferSegments");

    if (!m_locked) return YDS_ERROR_RETURN(ysError::YDS_BUFFER_NOT_LOCKED);

    if (segment1Size > m_lockSegment1Size) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);
    if (segment2Size > m_lockSegment2Size) return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);

    ResetLock();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::SetMode(Mode mode) {
    YDS_ERROR_DECLARE("SetMode");

    m_bufferMode = mode;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::SetVolume(float volume) {
    YDS_ERROR_DECLARE("SetVolume");

    m_volume = volume;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioSource::SetPan(float pan) {
    YDS_ERROR_DECLARE("SetPan");

    if (pan > 1.0f || pan < -1.0f)
        return YDS_ERROR_RETURN(ysError::YDS_OUT_OF_BOUNDS);

    m_pan = pan;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysAudioSource::ResetLock() {
    m_lockOffset = 0;
    m_lockSegment1Size = 0;
    m_lockSegment2Size = 0;
    m_locked = false;
}
