#include "../include/yds_sdl_input_device.h"

ysSdlInputDevice::ysSdlInputDevice() : ysInputDevice(Platform::Sdl, InputDeviceType::UNKNOWN) {
    /* void */
}

ysSdlInputDevice::ysSdlInputDevice(InputDeviceType inputDeviceType) : ysInputDevice(Platform::Sdl, inputDeviceType) {
    /* void */
}

ysSdlInputDevice::~ysSdlInputDevice() {
    /* void */
}
