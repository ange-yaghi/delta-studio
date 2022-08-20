#ifndef YDS_SDL_AUDIO_BUFFER_H
#define YDS_SDL_AUDIO_BUFFER_H

#include "yds_audio_buffer.h"

class ysSdlAudioBuffer : public ysAudioBuffer {
public:
    ysSdlAudioBuffer();
    virtual ~ysSdlAudioBuffer() override;

    virtual ysError Initialize(SampleOffset samples, const ysAudioParameters &parameters) override;
    virtual ysError EditBuffer(void *data) override;
    virtual ysError EditBufferSegment(void *data, SampleOffset samples, SampleOffset offset) override;
    virtual ysError Destroy() override;

    void *GetBuffer() const { return m_buffer; }

protected:
    void *m_buffer;
};

#endif /* YDS_SDL_AUDIO_BUFFER_H */
