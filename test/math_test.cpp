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

TEST(MathTest, QuatVecTransformWithMatrix) {
    ysQuaternion quat = ysMath::LoadVector(15.0f, 67.0f, 10.0f, -10.0f);
    ysVector translation = ysMath::LoadVector(10.0f, 56.0f, 67.0f, 0.0f);

    ysVector probe = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 1.0f);

    ysMatrix mat = ysMath::LoadMatrix(quat, translation);
    
    ysVector matTransform = ysMath::MatMult(mat, probe);
    ysVector quatTransform = ysMath::ExtendVector(ysMath::Add(ysMath::QuatTransform(quat, probe), translation));

    VecEq(matTransform, quatTransform);
}

TEST(MathTest, QuatVecTransformWithMatrixNoTranslation) {
    ysQuaternion quat = ysMath::LoadVector(15.0f, 67.0f, 10.0f, -10.0f);
    ysVector translation = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);

    ysVector probe = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 1.0f);

    ysMatrix mat = ysMath::LoadMatrix(quat, translation);

    ysVector matTransform = ysMath::MatMult(mat, probe);
    ysVector quatTransform = ysMath::ExtendVector(ysMath::Add(ysMath::QuatTransform(quat, probe), translation));

    VecEq(matTransform, quatTransform);
}

TEST(MathTest, QuatVecTransformWithMatrixNoTranslationSimple) {
    ysQuaternion quat = ysMath::LoadQuaternion(0.5f, ysMath::Constants::XAxis);
    ysVector translation = ysMath::LoadVector(0.0f, 0.0f, 0.0f, 0.0f);

    ysVector probe = ysMath::LoadVector(0.0f, 1.0f, 0.0f, 1.0f);

    ysMatrix mat = ysMath::LoadMatrix(quat, translation);

    ysVector matTransform = ysMath::MatMult(mat, probe);
    ysVector quatTransform = ysMath::ExtendVector(ysMath::Add(ysMath::QuatTransform(quat, probe), translation));

    VecEq(matTransform, quatTransform);
}

TEST(MathTest, SkewSymmetricMatrix) {
    ysVector a = ysMath::LoadVector(1.0f, 2.0f, 3.0f);
    ysVector b = ysMath::LoadVector(4.0f, 5.0f, 6.0f);
    ysMatrix mat = ysMath::SkewSymmetric(a);

    ysVector a_x_b_ref = ysMath::Cross(a, b);
    ysVector a_x_b_sym = ysMath::MatMult(mat, b);

    VecEq(a_x_b_ref, a_x_b_sym);
}

TEST(MathTest, CrossProductTest) {
    ysVector a = ysMath::LoadVector(1.0f, 2.0f, -3.0f);
    ysVector b = ysMath::LoadVector(-4.0f, 5.0f, 6.0f);
    
    a = ysMath::Normalize(a);
    b = ysMath::Normalize(b);

    ysVector c = ysMath::Cross(a, b);
    ysVector a_dot_c = ysMath::Dot(a, c);
    ysVector b_dot_c = ysMath::Dot(b, c);
    VecEq(a_dot_c, ysMath::Constants::Zero);
    VecEq(b_dot_c, ysMath::Constants::Zero);
}

TEST(MathTest, MatrixDet) {
    ysMatrix mat = ysMath::LoadMatrix(
        { 10.0f, 2.3f, 32.0f, 0.0f },
        { 4.0f, 15.0f, 6.0f, 0.0f },
        { 7.123f, 8.0f, 90.543f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f });

    ysVector det = ysMath::Det3x3(mat);

    VecEq(det, ysMath::LoadScalar(9971.7118f));
}

TEST(MathTest, MatrixInverse) {
    ysMatrix mat = ysMath::LoadMatrix(
        { 10.0f, 2.3f, 32.0f, 0.0f },
        { 4.0f, 15.0f, 6.0f, 0.0f },
        { 7.123f, 8.0f, 90.543f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f });

    ysMatrix inverse = ysMath::Inverse3x3(mat);
    ysMatrix I = ysMath::MatMult(mat, inverse);

    VecEq(I.rows[0], ysMath::Constants::IdentityRow1);
    VecEq(I.rows[1], ysMath::Constants::IdentityRow2);
    VecEq(I.rows[2], ysMath::Constants::IdentityRow3);
    VecEq(I.rows[3], ysMath::Constants::Zero);
}

TEST(MathTest, QuatTestAxisRotate) {
    ysQuaternion q = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::YAxis);
    q = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::XAxis);
    q = ysMath::LoadQuaternion(ysMath::Constants::PI / 2.0f, ysMath::Constants::ZAxis);
}
