#ifndef APTSIM_SOUND_SOURCE_H
#define APTSIM_SOUND_SOURCE_H

#include "../../../include/yds_base.h"
#include "../../../include/yds_audio_buffer.h"
#include "../../../include/yds_math.h"

class SoundSource : public ysObject {
public:
    SoundSource();
    ~SoundSource();

    void SetBuffer(ysAudioBuffer *buffer) { m_buffer = buffer; }

    void Update(ysVector &cameraPosition, ysVector &cameraDirection, ysVector &cameraUp);

    ysVector m_position;
    float m_volume;
    float m_falloffDistance;
    float m_falloffStart;

    bool m_falloff;

protected:
    ysAudioBuffer *m_buffer;
};

#endif /* APTSIM_SOUND_SOURCE_H */
