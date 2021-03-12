#include "../include/yds_mouse.h"

ysMouse::ysMouse() {
    Reset();
}

ysMouse::~ysMouse() {
    /* void */
}

void ysMouse::Reset() {
    for (int i = 0; i < (int)Button::Count; ++i) m_buttonStates[i] = ButtonState::Up;

    m_x = 0;
    m_y = 0;
    m_wheel = 0;

    m_osPosition_x = 0;
    m_osPosition_y = 0;
}

void ysMouse::UpdatePosition(int x, int y, bool delta) {
    if (delta) {
        m_x += x;
        m_y += y;
    }
    else {
        m_x = x;
        m_y = y;
    }
}

void ysMouse::UpdateWheel(int dwheel) {
    m_wheel += dwheel;
}

void ysMouse::UpdateButton(Button button, ButtonState state) {
    m_buttonStates[(int)button] = state;
}

void ysMouse::SetOsPosition(int x, int y) {
    m_osPosition_x = x;
    m_osPosition_y = y;
}

bool ysMouse::IsDown(Button button) const {
    return
        m_buttonStates[(int)button] == ButtonState::DownTransition ||
        m_buttonStates[(int)button] == ButtonState::Down;
}

bool ysMouse::ProcessMouseButton(Button button, ButtonState state) {
    ButtonState &currentState = m_buttonStates[(int)button];

    if (currentState != state) return false;

    if (currentState == ButtonState::UpTransition) currentState = ButtonState::Up;
    if (currentState == ButtonState::DownTransition) currentState = ButtonState::Down;

    return true;
}
