#ifndef APTSIM_LCD_CLOCK_H
#define APTSIM_LCD_CLOCK_H

#include "SceneObject.h"
#include "SceneManager.h"

#define HEX_EMPTY 0x0
#define HEX0 0x1
#define HEX1 0x2
#define HEX2 0x4
#define HEX3 0x8
#define HEX4 0x10
#define HEX5 0x20
#define HEX6 0x40

extern const int SegmentMap[];
extern const int SegmentDigitMap[];

struct LCD_Digit {
    SceneObject *m_segments[7];
};

class LCD_Clock {
public:
    LCD_Clock();
    ~LCD_Clock();

    LCD_Digit m_digits[4];
    SceneObject *m_colons[2];
    SceneObject *m_ampm;

    Material *m_onMaterial;
    Material *m_offMaterial;

    bool m_disableColons;
    bool m_disableAMPM;
    bool m_restrictedFirstDigit;
    bool m_displayFirstDigit0;

    void Create(SceneManager *manager, int index);
    void Display(int n1, int n2, bool pm = false);
};

#endif /* APTSIM_LCD_CLOCK_H */
