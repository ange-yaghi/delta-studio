#include "../include/yds_audio_buffer.h"

ysAudioBuffer::ysAudioBuffer(ysAudioSystemObject::API api) : ysAudioSystemObject("ysAudioBuffer", api) {
    m_size = 0;
}

ysAudioBuffer::~ysAudioBuffer() {
    /* void */
}

ysError ysAudioBuffer::Initialize(SampleOffset samples, const ysAudioParameters &parameters) {
    YDS_ERROR_DECLARE("Initialize");

    m_audioParameters = parameters;
    m_size = samples;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioBuffer::EditBuffer(void *data) {
    YDS_ERROR_DECLARE("EditBuffer");

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysAudioBuffer::EditBufferSegment(void *data, SampleOffset samples, SampleOffset offset) {
    YDS_ERROR_DECLARE("EditBufferSegment");

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}