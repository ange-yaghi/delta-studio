#ifndef DELTA_CORE_TEST_UTILITIES_H
#define DELTA_CORE_TEST_UTILITIES_H

#include <pch.h>

#include "../include/yds_math.h"

inline void VecEq(const ysVector &a, const ysVector &b, float epsilon = 0.0001f) {
    EXPECT_NEAR(ysMath::GetX(a), ysMath::GetX(b), epsilon);
    EXPECT_NEAR(ysMath::GetY(a), ysMath::GetY(b), epsilon);
    EXPECT_NEAR(ysMath::GetZ(a), ysMath::GetZ(b), epsilon);
    EXPECT_NEAR(ysMath::GetW(a), ysMath::GetW(b), epsilon);
}

#endif /* DELTA_CORE_TEST_UTILITIES_H */
