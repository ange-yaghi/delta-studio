#ifndef DELTA_PHYSICS_TEST_UTILITIES_H
#define DELTA_PHYSICS_TEST_UTILITIES_H

#include <pch.h>

#include "../include/delta_physics.h"

inline void VecEq(const ysVector &a, const ysVector &b) {
    EXPECT_NEAR(ysMath::GetX(a), ysMath::GetX(b), 0.0001f);
    EXPECT_NEAR(ysMath::GetY(a), ysMath::GetY(b), 0.0001f);
    EXPECT_NEAR(ysMath::GetZ(a), ysMath::GetZ(b), 0.0001f);
    EXPECT_NEAR(ysMath::GetW(a), ysMath::GetW(b), 0.0001f);
}

#endif /* DELTA_PHYSICS_TEST_UTILITIES_H */
