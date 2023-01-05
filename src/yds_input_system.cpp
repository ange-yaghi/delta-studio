#include "../include/yds_input_system.h"

#include "../include/yds_window_system.h"
#if PLATFORM_WIN32
#include "../include/yds_windows_input_system.h"
#endif
#if PLATFORM_SDL
#include "../include/yds_sdl_input_system.h"
#endif

ysInputSystem::ysInputSystem() : ysWindowSystemObject("INPUT_SYSTEM", Platform::Unknown) {
    m_windowSystem = nullptr;
    m_enableGlobalInput = false;
}

ysInputSystem::ysInputSystem(Platform platform) : ysWindowSystemObject("INPUT_SYSTEM", platform) {
    m_windowSystem = nullptr;
    m_enableGlobalInput = false;
}

ysInputSystem::~ysInputSystem() {
    /* void */
}

ysError ysInputSystem::CreateInputSystem(ysInputSystem **newInputSystem, Platform platform) {
    YDS_ERROR_DECLARE("CreateInputSystem");

    if (newInputSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);
    *newInputSystem = nullptr;

    switch (platform) {
    case Platform::Windows:
#if PLATFORM_WIN32
        *newInputSystem = new ysWindowsInputSystem();
#endif
        break;
    case Platform::Sdl:
#if PLATFORM_SDL
        *newInputSystem = new ysSdlInputSystem();
#endif
        break;
    }

    if (*newInputSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::InvalidParameter);

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysInputSystem::DestroyInputSystem(ysInputSystem *&inputSystem) {
    YDS_ERROR_DECLARE("DestroyInputSystem");

    delete inputSystem;
    inputSystem = nullptr;

    return YDS_ERROR_RETURN_STATIC(ysError::None);
}

ysError ysInputSystem::Initialize() {
    YDS_ERROR_DECLARE("Initialize");

    if (m_windowSystem == nullptr) return YDS_ERROR_RETURN(ysError::NoWindowSystem);

    YDS_NESTED_ERROR_CALL(CreateDevices());

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysInputSystem::AssignWindowSystem(ysWindowSystem *system) {
    YDS_ERROR_DECLARE("AssignWindowSystem");

    if (!CheckCompatibility(system)) return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);

    m_windowSystem = system;

    return YDS_ERROR_RETURN(ysError::None);
}

int ysInputSystem::GetNextDeviceID(ysInputDevice::InputDeviceType type) {
    const int deviceCount = GetDeviceCount();

    char *arr = new char[deviceCount];
    memset(arr, 0, sizeof(char) * deviceCount);

    for (int i = 0; i < deviceCount; i++) {
        if (m_inputDeviceArray.Get(i)->GetDeviceID() < deviceCount &&
            m_inputDeviceArray.Get(i)->GetType() == type &&
            m_inputDeviceArray.Get(i)->IsConnected())
        {
            arr[m_inputDeviceArray.Get(i)->GetDeviceID()] = 1;
        }
    }

    int next = deviceCount;
    for (int i = 0; i < deviceCount; i++) {
        if (arr[i] == 0) {
            next = i;
            break;
        }
    }

    delete[] arr;

    return next;
}

ysInputDevice *ysInputSystem::GetInputDevice(int id, ysInputDevice::InputDeviceType type) {
    const int deviceCount = GetDeviceCount();

    for (int i = 0; i < deviceCount; i++) {
        if (m_inputDeviceArray.Get(i)->GetDeviceID() == id && m_inputDeviceArray.Get(i)->GetType() == type) {
            return m_inputDeviceArray.Get(i);
        }
    }

    return CreateDevice(type, id);
}

ysError ysInputSystem::CheckDeviceStatus(ysInputDevice *device) {
    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysInputSystem::CheckAllDevices() {
    // This implementation is slow for most platforms
    // since the list of devices must be polled for each
    // game engine device.

    const int deviceCount = GetDeviceCount();

    // Reverse loop in the case some devices are deleted
    for (int i = deviceCount - 1; i >= 0; i--) {
        CheckDeviceStatus(m_inputDeviceArray.Get(i));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void ysInputSystem::RegisterDevice(ysInputDevice *device) {
    if (device->GetType() == ysInputDevice::InputDeviceType::KEYBOARD) {
        m_keyboardAggregator.RegisterKeyboard(device->GetAsKeyboard());
    }
    else if (device->GetType() == ysInputDevice::InputDeviceType::MOUSE) {
        m_mouseAggregator.RegisterMouse(device->GetAsMouse());
    }
}

void ysInputSystem::UnregisterDevice(ysInputDevice *device) {
    if (device->GetType() == ysInputDevice::InputDeviceType::KEYBOARD) {
        m_keyboardAggregator.DeleteKeyboard(device->GetAsKeyboard());
    }
    else if (device->GetType() == ysInputDevice::InputDeviceType::MOUSE) {
        m_mouseAggregator.DeleteMouse(device->GetAsMouse());
    }
}

void ysInputSystem::DisconnectDevice(ysInputDevice *device) {
    if (device->GetDependencyCount() <= 0) {
        // Delete device as no one is using it
        m_inputDeviceArray.Delete(device->GetIndex());
    }
    else {
        // Can't delete device as it is in use
        device->SetConnected(false);
    }
}

ysInputDevice *ysInputSystem::FindGenericSlot(ysInputDevice::InputDeviceType type) {
    const int deviceCount = GetDeviceCount();

    for (int i = 0; i < deviceCount; i++) {
        if (m_inputDeviceArray.Get(i)->IsGeneric() &&
            m_inputDeviceArray.Get(i)->GetType() == type &&
            !m_inputDeviceArray.Get(i)->IsConnected())
        {
            return m_inputDeviceArray.Get(i);
        }
    }

    // No generic slot
    return 0;
}
