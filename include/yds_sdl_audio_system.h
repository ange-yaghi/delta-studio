#ifndef YDS_SDL_AUDIO_SYSTEM_H
#define YDS_SDL_AUDIO_SYSTEM_H

#include "yds_audio_system.h"

class ysWindow;

class ysSdlAudioSystem : public ysAudioSystem {
public:
    ysSdlAudioSystem();
    virtual ~ysSdlAudioSystem() override;

    virtual void EnumerateDevices() override;
    virtual void ConnectDevice(ysAudioDevice *device, ysWindow *windowAssociation) override;
    virtual void ConnectDeviceConsole(ysAudioDevice *device) override;
    virtual void DisconnectDevice(ysAudioDevice *device) override;
};

#endif /* YDS_SDL_AUDIO_SYSTEM_H */
