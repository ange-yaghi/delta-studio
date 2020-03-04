#ifndef YDS_DIRECT_SOUND8_AUDIO_BUFFER_H
#define YDS_DIRECT_SOUND8_AUDIO_BUFFER_H

#include "yds_audio_buffer.h"

#include <dsound.h>

class ysDS8Device;

class ysDS8AudioBuffer : public ysAudioBuffer {
    friend ysDS8Device;

public:
    ysDS8AudioBuffer();
    virtual ~ysDS8AudioBuffer();

    virtual ysError LockEntireBuffer(void **buffer, SampleOffset *samples);
    virtual ysError LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset *size1, void **segment2, SampleOffset *size2);
    virtual ysError UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size);

    virtual ysError SetMode(AUDIO_BUFFER_MODE mode = MODE_PLAY);

    virtual ysError SetVolume(float volume);
    virtual ysError SetPan(float pan);

    virtual SampleOffset GetCurrentPosition();
    virtual SampleOffset GetCurrentWritePosition();

    virtual void Seek(SampleOffset offset);

protected:
    IDirectSoundBuffer *m_buffer;
};

#endif /* YDS_DIRECT_SOUND8_AUDIO_BUFFER_H */
