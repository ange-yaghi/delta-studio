#include "../include/expanding_spring.h"

dphysics::MSSExpandingSpring::MSSExpandingSpring() {
    m_expansionRate = 0.0f;
}

dphysics::MSSExpandingSpring::~MSSExpandingSpring() {
    /* void */
}

void dphysics::MSSExpandingSpring::Update(float timeDelta) {
    m_length += timeDelta * m_expansionRate;
}

float dphysics::MSSExpandingSpring::GetLength(float timeDelta) const {
    return m_length + timeDelta * m_expansionRate;
}
