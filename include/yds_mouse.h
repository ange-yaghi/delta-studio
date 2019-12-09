#ifndef YDS_MOUSE_H
#define YDS_MOUSE_H

#include "yds_keyboard.h"

class ysMouse {
public:
    enum BUTTON_CODE {
        BUTTON_LEFT,
        BUTTON_RIGHT,
        BUTTON_MIDDLE,

        BUTTON_1,
        BUTTON_2,
        BUTTON_3,
        BUTTON_4,
        BUTTON_5,

        BUTTON_COUNT
    };

public:
    ysMouse();
    ~ysMouse();

    void Reset();
    void UpdatePosition(int x, int y, bool delta = true);
    void UpdateWheel(int dwheel);
    void UpdateButton(BUTTON_CODE button, ysKey::KEY_STATE state);

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWheel() const { return m_wheel; }
    const ysKey *GetButton(BUTTON_CODE button) const { return &m_buttons[button]; }

    bool ProcessMouseButton(BUTTON_CODE button, ysKey::KEY_STATE state);

protected:
    int m_x;
    int m_y;
    int m_wheel;

protected:
    ysKey m_buttons[BUTTON_COUNT];
};

#endif /* YDS_MOUSE_H */
