#include <pch.h>

#include "../include/yds_math.h"

#include "utilities.h"

#include <fstream>

TEST(MathTest, QuatMul) {
    float a1 = 1, b1 = 2, c1 = 3, d1 = 4;
    float a2 = 5, b2 = 6, c2 = 7, d2 = 8;

    ysQuaternion q0 = ysMath::LoadVector(a1, b1, c1, d1);
    ysQuaternion q1 = ysMath::LoadVector(a2, b2, c2, d2);

    float r0 = a1 * a2 - b1 * b2 - c1 * c2 - d1 * d2;
    float r1 = a1 * b2 + b1 * a2 + c1 * d2 - d1 * c2;
    float r2 = a1 * c2 - b1 * d2 + c1 * a2 + d1 * b2;
    float r3 = a1 * d2 + b1 * c2 - c1 * b2 + d1 * a2;

    VecEq(ysMath::LoadVector(r0, r1, r2, r3), ysMath::QuatMultiply(q0, q1));
}

TEST(MathTest, QuatInvert) {
    ysQuaternion q = ysMath::LoadQuaternion(0.4f, ysMath::Constants::XAxis);

    VecEq(ysMath::QuatMultiply(q, ysMath::QuatInvert(q)), ysMath::Constants::QuatIdentity);
}

TEST(MathTest, QuatInvertInvalid) {
    ysQuaternion q = ysMath::LoadVector(1.0f, 5.0f, 6.0f, 7.0f);

    VecEq(ysMath::Normalize(ysMath::QuatMultiply(q, ysMath::QuatInvert(q))), ysMath::Constants::QuatIdentity);
}

TEST(MathTest, QuatVecTransformOrigin) {
    ysQuaternion quat = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::ZAxis);
    ysVector t = ysMath::QuatTransform(quat, ysMath::Constants::Zero);
    VecEq(t, ysMath::Constants::Zero);
}

TEST(MathTest, QuatVecTransformSimple) {
    ysQuaternion quat = ysMath::LoadVector(0.5f, 0.5f, 0.5f, 0.5f);
    ysVector v = ysMath::LoadVector(1.0f, 2.0f, 3.0f, 0.0f);

    ysVector t = ysMath::QuatTransform(quat, v);
    VecEq(t, ysMath::LoadVector(3.0f, 1.0f, 2.0f, 0.0f));
}

TEST(MathTest, QuatVecTransform) {
    ysQuaternion quat = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::ZAxis);
    ysVector v = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 0.0f);

    ysVector t = ysMath::QuatTransform(quat, v);
    VecEq(t, ysMath::LoadVector(-1.0f, 0.0f, 0.0f, 0.0f));
}

TEST(MathTest, QuatVecTransformScaled) {
    ysQuaternion quat = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::ZAxis);
    quat = ysMath::Mul(ysMath::LoadScalar(10.0f), quat);

    ysVector v = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 0.0f);

    ysVector t = ysMath::QuatTransform(quat, v);
    VecEq(t, ysMath::LoadVector(-1.0f, 0.0f, 0.0f, 0.0f));
}
