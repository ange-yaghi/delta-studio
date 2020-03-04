#include "../include/yds_input_system.h"

#include "../include/yds_window_system.h"
#include "../include/yds_windows_input_system.h"

ysInputSystem::ysInputSystem() : ysWindowSystemObject("INPUT_SYSTEM", Platform::UNKNOWN) {
	m_windowSystem = nullptr;
    m_supportMultiple = false;
}

ysInputSystem::ysInputSystem(Platform platform) : ysWindowSystemObject("INPUT_SYSTEM", platform) {
	m_windowSystem = nullptr;
    m_supportMultiple = false;
}

ysInputSystem::~ysInputSystem() {
    /* void */
}

ysError ysInputSystem::CreateInputSystem(ysInputSystem** newInputSystem, Platform platform) {
    YDS_ERROR_DECLARE("CreateInputSystem");

    if (newInputSystem == nullptr) return YDS_ERROR_RETURN_STATIC(ysError::YDS_INVALID_PARAMETER);
    *newInputSystem = nullptr;

    if (platform == Platform::UNKNOWN) return YDS_ERROR_RETURN_STATIC(ysError::YDS_INVALID_PARAMETER);

    switch (platform) {
    case Platform::WINDOWS:
        *newInputSystem = new ysWindowsInputSystem();
        break;
    }

    return YDS_ERROR_RETURN_STATIC(ysError::YDS_NO_ERROR);
}

ysError ysInputSystem::AssignWindowSystem(ysWindowSystem *system) {
	YDS_ERROR_DECLARE("AssignWindowSystem");

	if (!CheckCompatibility(system)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

	m_windowSystem = system;

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysInputSystem::CreateDevices(bool supportMultiple) {
	YDS_ERROR_DECLARE("CreateDevices");

	if (m_windowSystem == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_WINDOW_SYSTEM);
	m_supportMultiple = supportMultiple;

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

int ysInputSystem::GetNextDeviceID(ysInputDevice::InputDeviceType type) {
	int deviceCount = GetDeviceCount();

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

	for (int i = 0; i < deviceCount; i++) {
		if (arr[i] == 0) return i;
	}

	return deviceCount;
}

ysInputDevice *ysInputSystem::GetInputDevice(int id, ysInputDevice::InputDeviceType type) {
	if (!m_supportMultiple && id > 0) return NULL;

	int deviceCount = GetDeviceCount();

    for (int i = 0; i < deviceCount; i++) {
        if (m_inputDeviceArray.Get(i)->GetDeviceID() == id && m_inputDeviceArray.Get(i)->GetType() == type) {
            return m_inputDeviceArray.Get(i);
        }
    }

	return CreateDevice(type, id);
}

ysInputDevice *ysInputSystem::GetMainDevice(ysInputDevice::InputDeviceType type) {
	// Main device should always have ID=0
	ysInputDevice *mainDevice = GetInputDevice(0, type);

	return mainDevice;
}

ysError ysInputSystem::CheckDeviceStatus(ysInputDevice *device) {
	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysInputSystem::CheckAllDevices() {
	// This implementations is slow for most platforms
	// since the list of devices must be polled for each
	// game engine device.

	int deviceCount = GetDeviceCount();

	// Reverse loop in the case some devices are deleted
	for (int i = deviceCount - 1; i >= 0; i--) {
		CheckDeviceStatus(m_inputDeviceArray.Get(i));
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
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
	int deviceCount = GetDeviceCount();

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
