#ifndef YDS_AUDIO_SOURCE_H
#define YDS_AUDIO_SOURCE_H

#include "yds_audio_system_object.h"

#include "yds_audio_buffer.h"
#include "yds_audio_parameters.h"

class ysAudioSource : public ysAudioSystemObject {
    friend class ysAudioDevice;

public:
    enum class Mode {
        Play,
        PlayOnce,
        Loop,
        Stop,
        Undefined
    };

public:
    ysAudioSource();
    ysAudioSource(API api);
    virtual ~ysAudioSource();

    virtual ysError LockEntireBuffer(void **buffer, SampleOffset *samples);
    virtual ysError UnlockBuffer(void *buffer, SampleOffset samples);
    virtual ysError LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset *size1, void **segment2, SampleOffset *size2);
    virtual ysError UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size);

    virtual ysError SetDataBuffer(ysAudioBuffer *buffer);

    SampleOffset GetBufferSize() const { return m_bufferSize; }

    const ysAudioParameters *GetAudioParameters() const { return &m_audioParameters; }

    virtual Mode GetBufferMode() { return m_bufferMode; }
    virtual ysError SetMode(Mode mode = Mode::Play);

    virtual bool SetCurrentPosition(SampleOffset position) { return true; }
    virtual bool GetCurrentPosition(SampleOffset *position) { *position = 0; return true; }
    virtual bool GetCurrentWritePosition(SampleOffset *position) { *position = 0; return true; }

    virtual void Seek(SampleOffset offset) { (void)offset; }

    virtual ysError SetVolume(float volume);
    virtual ysError SetPan(float pan);

    void Release();

private:
    inline bool Destroyed() const { return m_destroyed; }

protected:
    virtual ysError Destroy();
    void ResetLock();

    ysAudioParameters m_audioParameters;
    ysAudioBuffer *m_dataBuffer;

    SampleOffset m_bufferSize;

    SampleOffset m_lockOffset;
    SampleOffset m_lockSegment1Size;
    SampleOffset m_lockSegment2Size;

    Mode m_bufferMode;

    float m_pan;
    float m_volume;
    float m_panVolume;

    std::atomic_bool m_destroyed = false;
    std::atomic_bool m_released = false;

private:
    bool m_locked;
};

#endif /* YDS_AUDIO_SOURCE_H */
