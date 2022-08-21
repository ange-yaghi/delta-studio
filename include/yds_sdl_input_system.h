#ifndef YDS_SDL_INPUT_SYSTEM_H
#define YDS_SDL_INPUT_SYSTEM_H

#include "yds_input_system.h"

union SDL_Event;

class ysSdlInputSystem : public ysInputSystem {
public:
    ysSdlInputSystem();
    virtual ~ysSdlInputSystem();

    void ProcessEvent(const SDL_Event &event);

protected:
    virtual ysError CreateDevices() override;
    virtual ysError CheckDeviceStatus(ysInputDevice *device) override;
    virtual ysError CheckAllDevices() override;
    virtual ysInputDevice *CreateDevice(ysInputDevice::InputDeviceType type, int id) override;

private:
    ysKeyboard *GetKeyboard();
    ysMouse *GetMouse();
};

#endif /* YDS_SDL_INPUT_SYSTEM_H */
