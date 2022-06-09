#include "../include/yds_ds8_audio_source.h"

#include "../include/yds_ds8_audio_buffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

ysDS8AudioSource::ysDS8AudioSource() : ysAudioSource(API::DirectSound8) {
    m_buffer = nullptr;
}

ysDS8AudioSource::~ysDS8AudioSource() {
    /* void */
}

ysError ysDS8AudioSource::LockEntireBuffer(void **buffer, SampleOffset *samples) {
    YDS_ERROR_DECLARE("LockEntireBuffer");

    HRESULT result;
    YDS_NESTED_ERROR_CALL( ysAudioSource::LockEntireBuffer(buffer, samples) );

    DWORD bytes;

    result = m_buffer->Lock(0, 0, buffer, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    if (FAILED(result)) {
        ResetLock();
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::UnlockBuffer(void *buffer, SampleOffset samples) {
    YDS_ERROR_DECLARE("UnlockBuffer");

    HRESULT result;
    YDS_NESTED_ERROR_CALL(ysAudioSource::UnlockBuffer(buffer, samples));

    DWORD totalSize = m_audioParameters.GetSizeFromSamples(samples);
    result = m_buffer->Unlock(buffer, totalSize, nullptr, 0);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset* size1, void **segment2, SampleOffset* size2) {
    YDS_ERROR_DECLARE("LockBufferSegment");

    HRESULT result;
    YDS_NESTED_ERROR_CALL( ysAudioSource::LockBufferSegment(offset, samples, segment1, size1, segment2, size2) );

    DWORD totalSize = m_audioParameters.GetSizeFromSamples(samples);
    DWORD byteOffset = m_audioParameters.GetSizeFromSamples(offset);

    DWORD segment1Size, segment2Size;

    result = m_buffer->Lock(byteOffset, totalSize, segment1, &segment1Size, segment2, &segment2Size, 0);

    if (FAILED(result)) {
        ResetLock();
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size) {
    YDS_ERROR_DECLARE("UnlockBufferSegments");

    HRESULT result;
    YDS_NESTED_ERROR_CALL( ysAudioSource::UnlockBufferSegments(segment1, segment1Size, segment2, segment2Size) );

    result = m_buffer->Unlock(segment1, segment1Size, segment2, segment2Size);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::SetMode(Mode mode) {
    YDS_ERROR_DECLARE("SetMode");

    YDS_NESTED_ERROR_CALL( ysAudioSource::SetMode(mode) );

    if (m_buffer == nullptr) return YDS_ERROR_RETURN(ysError::InvalidOperation);

    HRESULT result = 0;
    switch (mode) {
    case Mode::PlayOnce:
    case Mode::Play:
        result = m_buffer->Play(0, 0, 0);
        break;
    case Mode::Loop:
        result = m_buffer->Play(0, 0, DSBPLAY_LOOPING);
        break;
    case Mode::Stop:
        result = m_buffer->Stop();
        break;
    default:
        return YDS_ERROR_RETURN(ysError::None);
    }

    if (FAILED(result)) {
        m_bufferMode = Mode::Undefined;
        return YDS_ERROR_RETURN(ysError::InvalidOperation);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::SetDataBuffer(ysAudioBuffer *buffer) {
    YDS_ERROR_DECLARE("SetDataBuffer");

    YDS_NESTED_ERROR_CALL(ysAudioSource::SetDataBuffer(buffer));

    ysDS8AudioBuffer *ds8Buffer = static_cast<ysDS8AudioBuffer *>(buffer);

    void *target = nullptr;
    SampleOffset samples;
    YDS_NESTED_ERROR_CALL(LockEntireBuffer(&target, &samples));
    memcpy(target, ds8Buffer->GetBuffer(), ds8Buffer->GetBufferSize());

    YDS_NESTED_ERROR_CALL(UnlockBuffer(target, samples));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::SetVolume(float volume) {
    YDS_ERROR_DECLARE("SetVolume");

    YDS_NESTED_ERROR_CALL(ysAudioSource::SetVolume(volume) );

    float ratio = m_volume * m_panVolume;
    float db = 20 * log10f(ratio);

    HRESULT result = m_buffer->SetVolume(DSBVOLUME_MAX + max((LONG)(db * 100), DSBVOLUME_MIN));

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::InvalidOperation);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioSource::SetPan(float pan) {
    YDS_ERROR_DECLARE("SetPan");

    YDS_NESTED_ERROR_CALL(ysAudioSource::SetPan(pan) );

    HRESULT result;

    float angle = ((pan + 1) / 2.0f) * ((float)M_PI / 2.0f);
    float ratio = tanf(angle);
    float db = 20.0f * log10f(ratio);

    result = m_buffer->SetPan(min(max((LONG)(db * 1000), -10000), 10000));

    //result = m_buffer->SetPan((1 - weight) * DSBPAN_LEFT);

    if (FAILED(result)) {
        return YDS_ERROR_RETURN(ysError::InvalidOperation);
    }

    // Volume Compensation
    float mag = sqrtf(1.0f + 1.0f / (max(ratio, 1.0f / ratio)));
    m_panVolume = 1 / mag;
    SetVolume(m_volume);

    return YDS_ERROR_RETURN(ysError::None);
}

SampleOffset ysDS8AudioSource::GetCurrentPosition() {
    DWORD currentPosition;
    HRESULT result;

    if (m_buffer == nullptr) return 0;

    result = m_buffer->GetCurrentPosition(&currentPosition, NULL);
    if (FAILED(result)) return 0;

    return m_audioParameters.GetSamplesFromSize((unsigned int)currentPosition);
}

SampleOffset ysDS8AudioSource::GetCurrentWritePosition() {
    DWORD currentWrite;
    HRESULT result;

    if (m_buffer == nullptr) return 0;

    result = m_buffer->GetCurrentPosition(NULL, &currentWrite);
    if (FAILED(result)) return 0;

    return m_audioParameters.GetSamplesFromSize((unsigned int)currentWrite);
}

void ysDS8AudioSource::Seek(SampleOffset offset) {
    m_buffer->SetCurrentPosition(m_audioParameters.GetSizeFromSamples(offset));
}

ysError ysDS8AudioSource::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    YDS_NESTED_ERROR_CALL(ysAudioSource::Destroy());

    m_buffer->Release();
    m_buffer = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
