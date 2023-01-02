#include "../include/yds_input_device.h"

ysInputDevice::ysInputDevice() : ysWindowSystemObject("INPUT DEVICE", Platform::Unknown) {
    SetName("");

    m_type = InputDeviceType::UNKNOWN;
    m_connected = false;
    m_generic = true;
    m_dependencyCount = 0;

    m_deviceID = -1;

    // Clear specific pointers
    m_genericData = nullptr;
}

ysInputDevice::ysInputDevice(Platform platform, InputDeviceType type) : ysWindowSystemObject("INPUT DEVICE", platform) {
    SetName("");

    m_type = type;
    m_connected = false;
    m_generic = true;
    m_dependencyCount = 0;

    m_deviceID = -1;

    // Clear specific pointers
    m_genericData = nullptr;
}

ysInputDevice::~ysInputDevice() {
    Destroy();
}

void ysInputDevice::Destroy() {
    if (m_type == InputDeviceType::KEYBOARD) {
        delete m_keyboard;
    }
    else if (m_type == InputDeviceType::MOUSE) {
        delete m_mouse;
    }
    else if (m_type == InputDeviceType::CUSTOM) {
        // Delete custom
    }

    // Clear specific pointer
    m_generic = 0;
}

void ysInputDevice::SetName(std::string name) {
    m_name = name;
}

void ysInputDevice::SetType(ysInputDevice::InputDeviceType type) {
    if (m_type != type) Destroy();
    m_type = type;

    if (m_type == InputDeviceType::KEYBOARD) {
        m_keyboard = new ysKeyboard;
    }
    else if (m_type == InputDeviceType::MOUSE) {
        m_mouse = new ysMouse;
    }
    else if (m_type == InputDeviceType::CUSTOM) {
        // New custom
    }
}

ysKeyboard *ysInputDevice::GetAsKeyboard() {
    if (m_type == InputDeviceType::KEYBOARD) return m_keyboard;

    //RaiseError(false, "Illegal access of keyboard.\n");
    return nullptr;
}

ysMouse *ysInputDevice::GetAsMouse() {
    if (m_type == InputDeviceType::MOUSE) return m_mouse;

    //RaiseError(false, "Illegal access of mouse.\n");
    return nullptr;
}
