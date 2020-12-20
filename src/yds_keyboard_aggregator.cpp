#include "../include/yds_keyboard_aggregator.h"

ysKeyboardAggregator::ysKeyboardAggregator() {
    /* void */
}

ysKeyboardAggregator::~ysKeyboardAggregator() {
    /* void */
}

void ysKeyboardAggregator::RegisterKeyboard(ysKeyboard *keyboard) {
    m_keyboards.New() = keyboard;
}

void ysKeyboardAggregator::DeleteKeyboard(ysKeyboard *keyboard) {
    m_keyboards.Delete(m_keyboards.Find(keyboard));
}

bool ysKeyboardAggregator::IsKeyDown(ysKey::Code key) {
    const int keyboardCount = GetKeyboardCount();
    for (int i = 0; i < keyboardCount; ++i) {
        if (m_keyboards[i]->IsKeyDown(key)) return true;
    }

    return false;
}

bool ysKeyboardAggregator::ProcessKeyTransition(ysKey::Code key, ysKey::State state) {
    bool result = false;

    const int keyboardCount = GetKeyboardCount();
    for (int i = 0; i < keyboardCount; ++i) {
        result = result || m_keyboards[i]->ProcessKeyTransition(key, state);
    }

    return result;
}

void ysKeyboardAggregator::SetKeyState(ysKey::Code key, ysKey::State state, ysKey::Variation conf) {
    const int keyboardCount = GetKeyboardCount();
    for (int i = 0; i < keyboardCount; ++i) {
        m_keyboards[i]->SetKeyState(key, state, conf);
    }
}

const ysKey *ysKeyboardAggregator::GetKey(ysKey::Code key) const {
    return nullptr;
}
