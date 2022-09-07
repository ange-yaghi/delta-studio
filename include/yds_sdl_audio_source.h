#ifndef YDS_SDL_AUDIO_SOURCE_H
#define YDS_SDL_AUDIO_SOURCE_H

#include "yds_audio_source.h"

#include <mutex>

class ysSdlAudioDevice;

class ysSdlAudioSource : public ysAudioSource {
    friend ysSdlAudioDevice;

public:
    ysSdlAudioSource();
    virtual ~ysSdlAudioSource() override;

    virtual ysError LockEntireBuffer(void **buffer, SampleOffset *samples) override;
    virtual ysError UnlockBuffer(void *buffer, SampleOffset samples) override;
    virtual ysError LockBufferSegment(SampleOffset offset, SampleOffset samples, void **segment1, SampleOffset *size1, void **segment2, SampleOffset *size2) override;
    virtual ysError UnlockBufferSegments(void *segment1, SampleOffset segment1Size, void *segment2, SampleOffset segment2Size) override;

    virtual ysError SetMode(ysAudioSource::Mode mode = Mode::Play) override;

    virtual ysError SetDataBuffer(ysAudioBuffer *buffer) override;

    virtual ysError SetVolume(float volume) override;
    virtual ysError SetPan(float pan) override;

    virtual SampleOffset GetCurrentPosition() override;
    virtual SampleOffset GetCurrentWritePosition() override;

    virtual void Seek(SampleOffset offset) override;

    virtual ysError Destroy() override;

    void AddToBuffer(int16_t *audio, int frames);

private:
    size_t m_safeWriteFrames;

    // Lazy. Should be lock-free for perf here...
    std::mutex m_mutex;
    size_t m_readPosition;
};

#endif /* YDS_SDL_AUDIO_SOURCE_H */
