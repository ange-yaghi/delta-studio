#include "../include/yds_ds8_audio_buffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

ysDS8AudioBuffer::ysDS8AudioBuffer() : ysAudioBuffer(API_DIRECT_SOUND8) {
	m_buffer = NULL;
}

ysDS8AudioBuffer::~ysDS8AudioBuffer() {
    /* void */
}

ysError ysDS8AudioBuffer::LockEntireBuffer(void **buffer, SampleOffset *samples) {
	YDS_ERROR_DECLARE("LockEntireBuffer");

	HRESULT result;
	YDS_NESTED_ERROR_CALL( ysAudioBuffer::LockEntireBuffer(buffer, samples) );

	DWORD bytes;

	result = m_buffer->Lock(0, 0, buffer, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if (FAILED(result)) {
		ResetLock();
		return YDS_ERROR_RETURN(ysError::YDS_API_ERROR);
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysDS8AudioBuffer::LockBufferSegment(SampleOffset offset, SampleOffset samples, void** segment1, SampleOffset* size1, void** segment2, SampleOffset* size2) {
    YDS_ERROR_DECLARE("LockBufferSegment");

    HRESULT result;
    YDS_NESTED_ERROR_CALL(ysAudioBuffer::LockBufferSegment(offset, samples, segment1, size1, segment2, size2));

    DWORD totalSize = m_audioParameters.GetSizeFromSamples(samples);
    DWORD byteOffset = m_audioParameters.GetSizeFromSamples(offset);

    DWORD segment1Size, segment2Size;

    result = m_buffer->Lock(byteOffset, totalSize, segment1, &segment1Size, segment2, &segment2Size, 0);

    if (FAILED(result)) {
        ResetLock();
        return YDS_ERROR_RETURN(ysError::YDS_API_ERROR);
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysDS8AudioBuffer::UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size) {
	YDS_ERROR_DECLARE("UnlockBufferSegments");

	HRESULT result;
	YDS_NESTED_ERROR_CALL( ysAudioBuffer::UnlockBufferSegments(segment1, segment1Size, segment2, segment2Size) );

	result = m_buffer->Unlock(segment1, segment1Size, segment2, segment2Size);

	if (FAILED(result)) {
		return YDS_ERROR_RETURN(ysError::YDS_API_ERROR);
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysDS8AudioBuffer::SetMode(AUDIO_BUFFER_MODE mode) {
	YDS_ERROR_DECLARE("SetMode");

	YDS_NESTED_ERROR_CALL( ysAudioBuffer::SetMode(mode) );

	if (m_buffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_OPERATION);

    HRESULT result = 0;
	switch (mode) {
	case MODE_PLAY_ONCE:
	case MODE_PLAY:
		result = m_buffer->Play(0, 0, 0);
		break;
	case MODE_LOOP:
		result = m_buffer->Play(0, 0, DSBPLAY_LOOPING);
		break;
	case MODE_STOP:
		result = m_buffer->Stop();
		break;
    default:
        return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
	}

	if (FAILED(result)) {
		m_bufferMode = MODE_UNDEFINED;
        return YDS_ERROR_RETURN(ysError::YDS_INVALID_OPERATION);
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysDS8AudioBuffer::SetVolume(float volume) {
	YDS_ERROR_DECLARE("SetVolume");

	YDS_NESTED_ERROR_CALL(ysAudioBuffer::SetVolume(volume) );

	HRESULT result;

	float ratio = m_volume * m_panVolume;
	float db = 20 * log10f(ratio);

	result = m_buffer->SetVolume(DSBVOLUME_MAX + max((LONG)(db * 100), DSBVOLUME_MIN));

	if (FAILED(result)) {
		return YDS_ERROR_RETURN(ysError::YDS_INVALID_OPERATION);
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysDS8AudioBuffer::SetPan(float pan) {
	YDS_ERROR_DECLARE("SetPan");

	YDS_NESTED_ERROR_CALL(ysAudioBuffer::SetPan(pan) );

	HRESULT result;

	float angle = ((pan + 1) / 2.0f) * ((float)M_PI / 2.0f);
	float ratio = tanf(angle);
	float db = 20.0f * log10f(ratio);

	result = m_buffer->SetPan(min(max((LONG)(db * 1000), -10000), 10000));

	//result = m_buffer->SetPan((1 - weight) * DSBPAN_LEFT);

	if (FAILED(result)) {
		return YDS_ERROR_RETURN(ysError::YDS_INVALID_OPERATION);
	}

	// Volume Compensation
	float mag = sqrtf(1.0f + 1.0f / (max(ratio, 1.0f / ratio)));
	m_panVolume = 1 / mag;
	SetVolume(m_volume);

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

SampleOffset ysDS8AudioBuffer::GetCurrentPosition() {
	DWORD currentPosition;
	HRESULT result;

	if (m_buffer == NULL) return 0;

	result = m_buffer->GetCurrentPosition(&currentPosition, NULL);
	if (FAILED(result)) return 0;

	return m_audioParameters.GetSamplesFromSize((unsigned int)currentPosition);
}

SampleOffset ysDS8AudioBuffer::GetCurrentWritePosition() {
	DWORD currentWrite;
	HRESULT result;

	if (m_buffer == NULL) return 0;

	result = m_buffer->GetCurrentPosition(NULL, &currentWrite);
	if (FAILED(result)) return 0;

	return m_audioParameters.GetSamplesFromSize((unsigned int)currentWrite);
}

void ysDS8AudioBuffer::Seek(SampleOffset offset) {
	m_buffer->SetCurrentPosition(m_audioParameters.GetSizeFromSamples(offset));
}
