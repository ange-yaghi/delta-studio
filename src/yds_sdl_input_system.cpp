#include "../include/yds_sdl_input_system.h"

#include "../include/yds_sdl_input_device.h"
#include "../include/yds_key_maps.h"

#include <SDL2/SDL.h>

ysSdlInputSystem::ysSdlInputSystem() : ysInputSystem(Platform::Sdl) {
    /* void */
}

ysSdlInputSystem::~ysSdlInputSystem() {
    /* void */
}

void ysSdlInputSystem::ProcessEvent(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            auto &keyboardEvent = event.key;
            ysKeyboard *keyboard = GetKeyboard();

            ysKey::Code index = keyboard->GetKeyMap(keyboardEvent.keysym.scancode);
            const ysKey *key = keyboard->GetKey(index);

            ysKey::Variation newConf = key->m_configuration;
            ysKey::State newState = event.type == SDL_KEYDOWN
                ? ysKey::State::DownTransition
                : ysKey::State::UpTransition;

            keyboard->SetKeyState(index, newState, newConf);
            break;
        }

        case SDL_MOUSEMOTION: {
            auto &motionEvent = event.motion;
            ysMouse *mouse = GetMouse();

            mouse->UpdatePosition(motionEvent.x, motionEvent.y, false);
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            auto &buttonEvent = event.button;
            ysMouse *mouse = GetMouse();

            ysMouse::Button button;
            switch (buttonEvent.button) {
                case SDL_BUTTON_LEFT:   button = ysMouse::Button::Left;   break;
                case SDL_BUTTON_RIGHT:  button = ysMouse::Button::Right;  break;
                case SDL_BUTTON_MIDDLE: button = ysMouse::Button::Middle; break;
                case SDL_BUTTON_X1:     button = ysMouse::Button::Aux_1;  break;
                case SDL_BUTTON_X2:     button = ysMouse::Button::Aux_2;  break;
                default: return; // We don't know how to handle this button so bail
            }

            auto newState = event.type == SDL_MOUSEBUTTONDOWN
                ? ysMouse::ButtonState::DownTransition
                : ysMouse::ButtonState::UpTransition;

            mouse->UpdateButton(button, newState);
            break;
        }

        case SDL_MOUSEWHEEL: {
            auto &wheelEvent = event.wheel;
            ysMouse *mouse = GetMouse();

            mouse->UpdateWheel(wheelEvent.y);
            break;
        }

        default:
            // Ignore it
            break;
    }
}

ysError ysSdlInputSystem::CheckDeviceStatus(ysInputDevice *device) {
    YDS_ERROR_DECLARE("CheckDeviceStatus");

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlInputSystem::CheckAllDevices() {
    YDS_ERROR_DECLARE("CheckAllDevices");

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysSdlInputSystem::CreateDevices() {
    YDS_ERROR_DECLARE("CreateDevices");

    return YDS_ERROR_RETURN(ysError::None);
}

ysInputDevice *ysSdlInputSystem::CreateDevice(ysInputDevice::InputDeviceType type, int id) {
    ysSdlInputDevice *newDevice = m_inputDeviceArray.NewGeneric<ysSdlInputDevice>();

    const char *baseName;
    switch (type) {
    case ysInputDevice::InputDeviceType::CUSTOM:
        baseName = "HID";
        break;
    case ysInputDevice::InputDeviceType::KEYBOARD:
        baseName = "KEYBOARD";
        break;
    case ysInputDevice::InputDeviceType::MOUSE:
        baseName = "MOUSE";
        break;
    default:
        baseName = "";
    }

    char deviceName[128];
    snprintf(deviceName, sizeof(deviceName), "%s%0.3d", baseName, id);

    newDevice->SetName(deviceName);
    newDevice->SetType(type);
    newDevice->SetDeviceID(id);
    newDevice->SetGeneric(true);

    if (type == ysInputDevice::InputDeviceType::KEYBOARD) {
        // Load Keymap
        ysKeyboard *keyboard = newDevice->GetAsKeyboard();
        keyboard->RegisterKeyMap(ysKeyMaps::GetSdlKeyMap());
    }

    RegisterDevice(newDevice);

    return newDevice;
}

ysKeyboard *ysSdlInputSystem::GetKeyboard() {
    // Lazy, assume there's only 1 keyboard
    auto *keyboard = GetInputDevice(0, ysInputDevice::InputDeviceType::KEYBOARD);
    return keyboard->GetAsKeyboard();
}

ysMouse *ysSdlInputSystem::GetMouse() {
    // Lazy, assume there's only 1 mouse
    auto *device = GetInputDevice(1, ysInputDevice::InputDeviceType::MOUSE);
    return device->GetAsMouse();
}
