#include "../include/expanding_spring.h"

dbasic::MSSExpandingSpring::MSSExpandingSpring() {
    m_expansionRate = 0.0f;
}

dbasic::MSSExpandingSpring::~MSSExpandingSpring() {
    /* void */
}

void dbasic::MSSExpandingSpring::Update(float timeDelta) {
    m_length += timeDelta * m_expansionRate;
}

float dbasic::MSSExpandingSpring::GetLength(float timeDelta) const {
    return m_length + timeDelta * m_expansionRate;
}
