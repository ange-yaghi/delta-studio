#ifndef YDS_AUDIO_BUFFER_H
#define YDS_AUDIO_BUFFER_H

#include "yds_audio_system_object.h"

#include "yds_audio_parameters.h"

class ysAudioBuffer : public ysAudioSystemObject {
public:
    ysAudioBuffer(ysAudioSystemObject::API api);
    virtual ~ysAudioBuffer();

    virtual ysError Initialize(SampleOffset samples, const ysAudioParameters &parameters);
    virtual ysError EditBuffer(void *data);
    virtual ysError EditBufferSegment(void *data, SampleOffset samples, SampleOffset offset);

    virtual SampleOffset GetBufferSize() const;

    const ysAudioParameters *GetAudioParameters() const { return &m_audioParameters; }

protected:
    ysAudioParameters m_audioParameters;
    SampleOffset m_size;
};

#endif /* YDS_AUDIO_BUFFER_H */
