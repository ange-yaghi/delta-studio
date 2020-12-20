#include "../include/yds_mouse_aggregator.h"

ysMouseAggregator::ysMouseAggregator() {
    /* void */
}

ysMouseAggregator::~ysMouseAggregator() {
    /* void */
}

void ysMouseAggregator::RegisterMouse(ysMouse *mouse) {
    m_mice.New() = mouse;
}

void ysMouseAggregator::DeleteMouse(ysMouse *mouse) {
    m_mice.Delete(m_mice.Find(mouse));
}

int ysMouseAggregator::GetOsPositionX() const {
    if (m_mice.GetNumObjects() == 0) return 0;

    return m_mice[0]->GetOsPositionX();
}

int ysMouseAggregator::GetOsPositionY() const {
    if (m_mice.GetNumObjects() == 0) return 0;

    return m_mice[0]->GetOsPositionY();
}

int ysMouseAggregator::GetX() const {
    int aggregateX = 0;
    const int mouseCount = m_mice.GetNumObjects();
    for (int i = 0; i < mouseCount; ++i) {
        aggregateX += m_mice[i]->GetX();
    }

    return aggregateX;
}

int ysMouseAggregator::GetY() const {
    int aggregateY = 0;
    const int mouseCount = m_mice.GetNumObjects();
    for (int i = 0; i < mouseCount; ++i) {
        aggregateY += m_mice[i]->GetY();
    }

    return aggregateY;
}

int ysMouseAggregator::GetWheel() const {
    int aggregateWheel = 0;
    const int mouseCount = m_mice.GetNumObjects();
    for (int i = 0; i < mouseCount; ++i) {
        aggregateWheel += m_mice[i]->GetWheel();
    }

    return aggregateWheel;
}

bool ysMouseAggregator::IsDown(Button button) const {
    const int mouseCount = m_mice.GetNumObjects();
    for (int i = 0; i < mouseCount; ++i) {
        if (m_mice[i]->IsDown(button)) return true;
    }

    return false;
}

bool ysMouseAggregator::ProcessMouseButton(Button button, ButtonState state) {
    bool result = false;

    const int mouseCount = m_mice.GetNumObjects();
    for (int i = 0; i < mouseCount; ++i) {
        result = result || m_mice[i]->ProcessMouseButton(button, state);
    }

    return result;
}
