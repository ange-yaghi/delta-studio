#ifndef YDS_DS8_DEVICE_H
#define YDS_DS8_DEVICE_H

#include "yds_audio_device.h"

class ysDS8System;

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

#include <AudioToolbox/AudioToolbox.h>

//class ysCoreAudioDevice : public ysAudioDevice {
//    friend ysCoreAudioSystem;
//
//public:
//    ysCoreAudioDevice();
//    virtual ~ysCoreAudioDevice();
//
//    virtual ysError CreateBuffer(const ysAudioParameters *parameters, SampleOffset size, ysAudioBuffer **buffer) override;
//
//    virtual ysError CreateSource(const ysAudioParameters *parameters, SampleOffset size, ysAudioSource **source) override;
//    virtual ysError CreateSource(ysAudioBuffer *sourceBuffer, ysAudioSource **source) override;
//
//    virtual void UpdateAudioSources();
//
//private:
//    AudioDeviceID m_deviceID;
//    AudioStreamBasicDescription m_streamFormat;
//    AudioUnit m_outputUnit;
//};

#elif defined(_WIN64)

#include <dsound.h>

class ysDS8Device : public ysAudioDevice {
    friend ysDS8System;

public:
    ysDS8Device();
    virtual ~ysDS8Device();

    virtual ysError CreateBuffer(const ysAudioParameters *parameters, SampleOffset size, ysAudioBuffer **buffer) override;

    virtual ysError CreateSource(const ysAudioParameters *parameters, SampleOffset size, ysAudioSource **source) override;
    virtual ysError CreateSource(ysAudioBuffer *sourceBuffer, ysAudioSource **source) override;

    virtual void UpdateAudioSources();

private:
    GUID m_guid;
    IDirectSound8 *m_device;
};

#endif /* Windows */

#endif /* YDS_DS8_DEVICE_H */
