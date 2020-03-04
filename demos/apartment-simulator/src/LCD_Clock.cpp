#include "../include/LCD_Clock.h"

const int SegmentDigitMap[] = { HEX0 | HEX1 | HEX2 | HEX3 | HEX4 | HEX5, // 0
                                HEX_EMPTY | HEX1 | HEX2, // 1
                                HEX0 | HEX1 | HEX_EMPTY | HEX3 | HEX4 | HEX_EMPTY | HEX6, // 2
                                HEX0 | HEX1 | HEX2 | HEX3 | HEX_EMPTY | HEX_EMPTY | HEX6, // 3
                                HEX_EMPTY | HEX1 | HEX2 | HEX_EMPTY | HEX_EMPTY | HEX5 | HEX6, // 4
                                HEX0 | HEX_EMPTY | HEX2 | HEX3 | HEX_EMPTY | HEX5 | HEX6, // 5
                                HEX0 | HEX_EMPTY | HEX2 | HEX3 | HEX4 | HEX5 | HEX6, // 6
                                HEX0 | HEX1 | HEX2, // 7
                                HEX0 | HEX1 | HEX2 | HEX3 | HEX4 | HEX5 | HEX6, // 8
                                HEX0 | HEX1 | HEX2 | HEX3 | HEX_EMPTY | HEX5 | HEX6 // 9
};

const int SegmentMap[] = { HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, HEX6 };

LCD_Clock::LCD_Clock() {
    m_colons[0] = m_colons[1] = HEX_EMPTY;

    m_onMaterial = nullptr;
    m_offMaterial = nullptr;

    m_disableColons = false;
    m_restrictedFirstDigit = false;
    m_disableAMPM = true;
    m_displayFirstDigit0 = false;
}

LCD_Clock::~LCD_Clock() {
    /* void */
}

void LCD_Clock::Create(SceneManager *manager, int index) {
    char buffer[64];

    for (int d = 0; d < 4; d++) {
        for (int i = 0; i < 7; i++) {
            sprintf_s(buffer, 64, "D%d_HEX%d_%.3d", d, i, index);
            m_digits[d].m_segments[i] = manager->FindObject(buffer);
        }
    }

    sprintf_s(buffer, 64, "AMPM_Dot%.3d", index);
    m_ampm = manager->FindObject(buffer);

    for (int i = 0; i < 2; i++) {
        sprintf_s(buffer, 64, "HEX_Colon%d_%.3d", i, index);
        m_colons[i] = manager->FindObject(buffer);
    }
}

void LCD_Clock::Display(int n1, int n2, bool pm) {
    m_ampm->m_hidden = m_disableAMPM;
    m_colons[0]->m_hidden = m_colons[1]->m_hidden = m_disableColons;
    if (m_restrictedFirstDigit) {
        for (int i = 0; i < 7; i++) {
            m_digits[3].m_segments[i]->m_hidden = (!(SegmentMap[i] & SegmentDigitMap[1]));
        }
    }

    m_ampm->SetMaterial((pm) ? m_onMaterial : m_offMaterial);
    for (int i = 0; i < 2; i++) {
        m_colons[i]->SetMaterial(m_onMaterial);
    }

    int digits[4];
    digits[0] = n2 % 10; n2 /= 10;
    digits[1] = n2 % 10;

    digits[2] = n1 % 10; n1 /= 10;
    digits[3] = n1 % 10;

    for (int d = 0; d < 4; d++) {
        for (int i = 0; i < 7; i++) {
            if ((d == 3 && !m_displayFirstDigit0 && digits[3] != 0) ||
                d != 3 ||
                m_displayFirstDigit0) {

                m_digits[d].m_segments[i]->SetMaterial(((SegmentMap[i] & SegmentDigitMap[digits[d]])) ? m_onMaterial : m_offMaterial);

            }
            else m_digits[d].m_segments[i]->SetMaterial(m_offMaterial);
        }
    }
}
