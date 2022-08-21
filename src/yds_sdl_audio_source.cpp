#include "../include/yds_sdl_audio_source.h"

#include "../include/yds_sdl_audio_buffer.h"

#include <cmath>

ysSdlAudioSource::ysSdlAudioSource() : ysAudioSource(API::Sdl) {
    m_readPosition = 0;
    m_queuedSize = 0;
}

ysSdlAudioSource::~ysSdlAudioSource() {
    /* void */
}

ysError ysSdlAudioSource::LockEntireBuffer(void **buffer, SampleOffset *samples) {
    YDS_ERROR_DECLARE("LockEntireBuffer");

    YDS_NESTED_ERROR_CALL( ysAudioSource::LockEntireBuffer(buffer, samples) );

    ysSdlAudioBuffer *sdlBuffer = static_cast<ysSdlAudioBuffer *>(m_dataBuffer);
    *buffer = sdlBuffer->GetBuffer();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::UnlockBuffer(void *buffer, SampleOffset samples) {
    YDS_ERROR_DECLARE("UnlockBuffer");

    YDS_NESTED_ERROR_CALL(ysAudioSource::UnlockBuffer(buffer, samples));

    m_queuedSize += m_audioParameters.GetSizeFromSamples(samples);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset* size1, void **segment2, SampleOffset* size2) {
    YDS_ERROR_DECLARE("LockBufferSegment");

    YDS_NESTED_ERROR_CALL( ysAudioSource::LockBufferSegment(offset, samples, segment1, size1, segment2, size2) );

    size_t byteOffset = m_audioParameters.GetSizeFromSamples(offset);

    ysSdlAudioBuffer *sdlBuffer = static_cast<ysSdlAudioBuffer *>(m_dataBuffer);
    *segment1 = (char*)sdlBuffer->GetBuffer() + byteOffset;
    *segment2 = sdlBuffer->GetBuffer();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size) {
    YDS_ERROR_DECLARE("UnlockBufferSegments");

    YDS_NESTED_ERROR_CALL( ysAudioSource::UnlockBufferSegments(segment1, segment1Size, segment2, segment2Size) );

    m_queuedSize += m_audioParameters.GetSizeFromSamples(segment1Size + segment2Size);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::SetMode(Mode mode) {
    YDS_ERROR_DECLARE("SetMode");

    YDS_NESTED_ERROR_CALL( ysAudioSource::SetMode(mode) );

    // TODO

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::SetDataBuffer(ysAudioBuffer *buffer) {
    YDS_ERROR_DECLARE("SetDataBuffer");

    YDS_NESTED_ERROR_CALL(ysAudioSource::SetDataBuffer(buffer));

    ysSdlAudioBuffer *sdlBuffer = static_cast<ysSdlAudioBuffer *>(buffer);

    void *target = nullptr;
    SampleOffset samples;
    YDS_NESTED_ERROR_CALL(LockEntireBuffer(&target, &samples));
    memcpy(target, sdlBuffer->GetBuffer(), sdlBuffer->GetBufferSize());

    YDS_NESTED_ERROR_CALL(UnlockBuffer(target, samples));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::SetVolume(float volume) {
    YDS_ERROR_DECLARE("SetVolume");

    // TODO

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioSource::SetPan(float pan) {
    YDS_ERROR_DECLARE("SetPan");

    // TODO

    return YDS_ERROR_RETURN(ysError::None);
}

SampleOffset ysSdlAudioSource::GetCurrentPosition() {
    const size_t bufferSize = m_audioParameters.GetSizeFromSamples(m_bufferSize);
    const size_t readPosition = m_readPosition % bufferSize;
    return m_audioParameters.GetSamplesFromSize(readPosition);
}

SampleOffset ysSdlAudioSource::GetCurrentWritePosition() {
    const size_t bufferSize = m_audioParameters.GetSizeFromSamples(m_bufferSize);
    const size_t writePosition = (m_readPosition + m_queuedSize) % bufferSize;

    // HACK: add a bodge so that code actually runs
    const size_t bodgeSamples = 441;
    return m_audioParameters.GetSamplesFromSize(writePosition) + bodgeSamples;
}

void ysSdlAudioSource::Seek(SampleOffset offset) {
    // TODO
}

ysError ysSdlAudioSource::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    YDS_NESTED_ERROR_CALL(ysAudioSource::Destroy());

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysSdlAudioSource::TryLockForRead(void **segment1, size_t *length1, void **segment2, size_t *length2)
{
    // TODO: add a way to check this is safe
    //if (m_locked) return false;

    ysSdlAudioBuffer *sdlBuffer = static_cast<ysSdlAudioBuffer *>(m_dataBuffer);
    auto *buffer = (uint8_t *)sdlBuffer->GetBuffer();
    const size_t bufferSize = m_audioParameters.GetSizeFromSamples(m_bufferSize);
    const size_t startOffset = m_readPosition % bufferSize;

    // First segment start is simple
    *segment1 = buffer + startOffset;

    if (startOffset + m_queuedSize <= bufferSize) {
        // Everything is contiguous, so no second segment
        *length1 = m_queuedSize;
        *segment2 = nullptr;
        *length2 = 0;
    } else {
        // Need to split it up into 2 segments
        *length1 = bufferSize - startOffset;
        *segment2 = buffer;
        *length2 = startOffset + m_queuedSize - bufferSize;
    }

    return true;
}

void ysSdlAudioSource::UnlockForRead()
{
    const size_t bufferSize = m_audioParameters.GetSizeFromSamples(m_bufferSize);
    m_readPosition += m_queuedSize;
    m_queuedSize = 0;
}
