#include "../include/yds_ds8_audio_buffer.h"

ysDS8AudioBuffer::ysDS8AudioBuffer() : ysAudioBuffer(API::DirectSound8) {
    m_buffer = nullptr;
}

ysDS8AudioBuffer::~ysDS8AudioBuffer() {
    /* void */
}

ysError ysDS8AudioBuffer::Initialize(SampleOffset samples, const ysAudioParameters &parameters) {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(ysAudioBuffer::Initialize(samples, parameters));

    m_buffer = malloc(parameters.GetSizeFromSamples(samples));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioBuffer::EditBuffer(void *data) {
    YDS_ERROR_DECLARE("EditBuffer");

    memcpy(m_buffer, data, m_audioParameters.GetSizeFromSamples(m_samples));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioBuffer::EditBufferSegment(void *data, SampleOffset samples, SampleOffset offset) {
    YDS_ERROR_DECLARE("EditBufferSegment");

    size_t offsetBytes = m_audioParameters.GetSizeFromSamples(offset);
    size_t totalBytes = m_audioParameters.GetSizeFromSamples(samples);

    memcpy((char *)data + offsetBytes, data, samples);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysDS8AudioBuffer::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    free(m_buffer);
    m_buffer = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
