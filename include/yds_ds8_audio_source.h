#ifndef YDS_DIRECT_SOUND8_AUDIO_SOURCE_H
#define YDS_DIRECT_SOUND8_AUDIO_SOURCE_H

#include "yds_audio_source.h"

#include <dsound.h>

class ysDS8Device;

class ysDS8AudioSource : public ysAudioSource {
    friend ysDS8Device;

public:
    ysDS8AudioSource();
    virtual ~ysDS8AudioSource();

    virtual ysError LockEntireBuffer(void **buffer, SampleOffset *samples);
    virtual ysError UnlockBuffer(void *buffer, SampleOffset samples);
    virtual ysError LockBufferSegment(SampleOffset offset, SampleOffset samples,
                                      void **segment1, SampleOffset *size1,
                                      void **segment2, SampleOffset *size2);
    virtual ysError UnlockBufferSegments(void *segment1,
                                         SampleOffset segment1Size,
                                         void *segment2,
                                         SampleOffset segment2Size);

    virtual ysError SetMode(ysAudioSource::Mode mode = Mode::Play);
    virtual Mode GetBufferMode();

    virtual ysError SetDataBuffer(ysAudioBuffer *buffer);

    virtual ysError SetVolume(float volume);
    virtual ysError SetPan(float pan);

    virtual bool SetCurrentPosition(SampleOffset position) override;
    virtual bool GetCurrentPosition(SampleOffset *position) override;
    virtual bool GetCurrentWritePosition(SampleOffset *position) override;

    virtual void Seek(SampleOffset offset);

    virtual ysError Destroy();

private:
    ysError UpdateStatus();
    ysError RestoreBuffer();

private:
    bool m_lost;
    IDirectSoundBuffer8 *m_buffer;
};

#endif /* YDS_DIRECT_SOUND8_AUDIO_SOURCE_H */
