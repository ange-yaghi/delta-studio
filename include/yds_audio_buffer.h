#ifndef YDS_AUDIO_BUFFER_H
#define YDS_AUDIO_BUFFER_H

#include "yds_audio_system_object.h"

#include "yds_audio_parameters.h"

class ysAudioBuffer : public ysAudioSystemObject {
    friend class ysAudioDevice;

public:
    enum AUDIO_BUFFER_MODE {
        MODE_PLAY,
        MODE_PLAY_ONCE,
        MODE_LOOP,
        MODE_STOP,
        MODE_UNDEFINED
    };

public:
    ysAudioBuffer();
    ysAudioBuffer(AUDIO_SYSTEM_API API);
    virtual ~ysAudioBuffer();

    virtual ysError LockEntireBuffer(void **buffer, SampleOffset *samples);
    virtual ysError LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset *size1, void **segment2, SampleOffset *size2);
    virtual ysError UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size);

    SampleOffset GetBufferSize() const { return m_bufferSize; }

    const ysAudioParameters *GetAudioParameters() const { return &m_audioParameters; }

    AUDIO_BUFFER_MODE GetBufferMode() const { return m_bufferMode; }

    virtual ysError SetMode(AUDIO_BUFFER_MODE mode = MODE_PLAY);

    virtual SampleOffset GetCurrentPosition() { return 0; }
    virtual SampleOffset GetCurrentWritePosition() { return 0; }

    virtual void Seek(SampleOffset offset) { (void)offset; }

    virtual ysError SetVolume(float volume);
    virtual ysError SetPan(float pan);

protected:
    void ResetLock();

    ysAudioParameters m_audioParameters;

    SampleOffset m_bufferSize;

    SampleOffset m_lockOffset;
    SampleOffset m_lockSegment1Size;
    SampleOffset m_lockSegment2Size;

    AUDIO_BUFFER_MODE m_bufferMode;

    float m_pan;
    float m_volume;
    float m_panVolume;

private:
    bool m_locked;
};

#endif /* YDS_AUDIO_BUFFER_H */
