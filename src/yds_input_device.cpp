#include "yds_input_device.h"

#include <cstring>

ysInputDevice::ysInputDevice()
    : ysWindowSystemObject("INPUT DEVICE", Platform::Unknown)
    , m_type(InputDeviceType::UNKNOWN)
    , m_name({0})
    , m_deviceID(-1)
    , m_dependencyCount(0)
    , m_genericData(nullptr)
    , m_connected(false)
    , m_generic(true)
    , m_virtual(false)
{
    SetName("");
}

ysInputDevice::ysInputDevice(Platform platform, InputDeviceType type)
    : ysWindowSystemObject("INPUT DEVICE", platform) 
    , m_type(InputDeviceType::UNKNOWN)
    , m_name({0})
    , m_deviceID(-1)
    , m_dependencyCount(0)
    , m_genericData(nullptr)
    , m_connected(false)
    , m_generic(true)
    , m_virtual(false)
{
    SetName("");
}

ysInputDevice::~ysInputDevice()
{
    Destroy();
}

void ysInputDevice::Destroy()
{
    switch (m_type)
    {
    case InputDeviceType::KEYBOARD: delete m_keyboard; break;
    case InputDeviceType::MOUSE:    delete m_mouse;    break;   
    case InputDeviceType::CUSTOM:   break;
    default: break;
    }

    m_generic = false;
}

void ysInputDevice::SetName(const char *name)
{
    strcpy_s(m_name, sizeof(m_name), name);
}

void ysInputDevice::SetType(ysInputDevice::InputDeviceType type)
{
    if (m_type != type) 
    {
        Destroy();
    }

    m_type = type;

    switch (m_type)
    {
    case InputDeviceType::KEYBOARD: m_keyboard = new ysKeyboard; break;
    case InputDeviceType::MOUSE:    m_mouse    = new ysMouse;    break;   
    case InputDeviceType::CUSTOM:   break;
    default: break;
    }
}

ysKeyboard *ysInputDevice::GetAsKeyboard()
{
    return (m_type == InputDeviceType::KEYBOARD) ? m_keyboard : nullptr;
}

ysMouse *ysInputDevice::GetAsMouse()
{
    return (m_type == InputDeviceType::MOUSE) ? m_mouse : nullptr;
}
