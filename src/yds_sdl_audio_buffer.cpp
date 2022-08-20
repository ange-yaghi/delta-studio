#include "../include/yds_sdl_audio_buffer.h"

// TODO: this is identical to the DS8 impl...

ysSdlAudioBuffer::ysSdlAudioBuffer() : ysAudioBuffer(API::Sdl) {
    m_buffer = nullptr;
}

ysSdlAudioBuffer::~ysSdlAudioBuffer() {
    /* void */
}

ysError ysSdlAudioBuffer::Initialize(SampleOffset samples, const ysAudioParameters &parameters) {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(ysAudioBuffer::Initialize(samples, parameters));

    m_buffer = calloc(parameters.GetSizeFromSamples(samples), 1);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioBuffer::EditBuffer(void *data) {
    YDS_ERROR_DECLARE("EditBuffer");

    memcpy(m_buffer, data, m_audioParameters.GetSizeFromSamples(m_samples));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioBuffer::EditBufferSegment(void *data, SampleOffset samples, SampleOffset offset) {
    YDS_ERROR_DECLARE("EditBufferSegment");

    size_t offsetBytes = m_audioParameters.GetSizeFromSamples(offset);
    size_t totalBytes = m_audioParameters.GetSizeFromSamples(samples);

    memcpy((char *)data + offsetBytes, data, samples);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlAudioBuffer::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    free(m_buffer);
    m_buffer = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
