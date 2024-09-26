#ifndef YDS_DS8_DEVICE_H
#define YDS_DS8_DEVICE_H

#include "yds_audio_device.h"

#include <dsound.h>

class ysDS8System;

class ysDS8Device : public ysAudioDevice {
    friend ysDS8System;

public:
    ysDS8Device();
    virtual ~ysDS8Device();

    virtual ysError CreateSource(const ysAudioParameters *parameters,
                                 SampleOffset size,
                                 ysAudioSource **source) override;
    virtual ysError CreateSource(ysAudioBuffer *sourceBuffer,
                                 ysAudioSource **source) override;

    virtual void UpdateAudioSources() override;
    ysError Prime();

private:
    ysError CreateBuffer(const ysAudioParameters *parameters, SampleOffset size,
                         IDirectSoundBuffer8 **target);

private:
    GUID m_guid;
    IDirectSound8 *m_device;
};

#endif /* YDS_DS8_DEVICE_H */
