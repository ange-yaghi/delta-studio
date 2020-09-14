#ifndef YDS_MOUSE_H
#define YDS_MOUSE_H

#include "yds_keyboard.h"

class ysMouse {
public:
    enum class Button {
        Left,
        Right,
        Middle,

        Aux_1,
        Aux_2,
        Aux_3,
        Aux_4,
        Aux_5,

        Count
    };

public:
    ysMouse();
    ~ysMouse();

    void Reset();
    void UpdatePosition(int x, int y, bool delta = true);
    void UpdateWheel(int dwheel);
    void UpdateButton(Button button, ysKey::KEY_STATE state);
    void SetOsPosition(int x, int y);

    int GetOsPositionX() const { return m_osPosition_x; }
    int GetOsPositionY() const { return m_osPosition_y; }

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWheel() const { return m_wheel; }
    const ysKey *GetButton(Button button) const { return &m_buttons[(int)button]; }

    bool ProcessMouseButton(Button button, ysKey::KEY_STATE state);

protected:
    int m_x;
    int m_y;
    int m_wheel;

    int m_osPosition_x;
    int m_osPosition_y;

protected:
    ysKey m_buttons[(int)Button::Count];
};

#endif /* YDS_MOUSE_H */
