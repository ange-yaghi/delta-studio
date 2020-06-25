#include "../include/yds_mouse.h"

ysMouse::ysMouse() {
    m_x = 0;
    m_y = 0;
    m_wheel = 0;
}

ysMouse::~ysMouse() {
    /* void */
}

void ysMouse::Reset() {
    for (int i = 0; i < (int)Button::Count; i++) m_buttons[i].Reset();

    m_x = 0;
    m_y = 0;
    m_wheel = 0;
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

void ysMouse::UpdateButton(Button button, ysKey::KEY_STATE state) {
    m_buttons[(int)button].m_state = state;
}

bool ysMouse::ProcessMouseButton(Button button, ysKey::KEY_STATE state) {
    if (m_buttons[(int)button].m_state != state) return false;

    if (m_buttons[(int)button].m_state == ysKey::KEY_UP_TRANS) m_buttons[(int)button].m_state = ysKey::KEY_UP;
    if (m_buttons[(int)button].m_state == ysKey::KEY_DOWN_TRANS) m_buttons[(int)button].m_state = ysKey::KEY_DOWN;

    return true;
}
