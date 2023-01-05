#ifndef YDS_SDL_INPUT_DEVICE_H
#define YDS_SDL_INPUT_DEVICE_H

#include "yds_input_device.h"

class ysSdlInputSystem;

class ysSdlInputDevice : public ysInputDevice {
    friend ysSdlInputSystem;

public:
    ysSdlInputDevice();
    ysSdlInputDevice(InputDeviceType type);
    ~ysSdlInputDevice();
};

#endif /* YDS_SDL_INPUT_DEVICE_H */
