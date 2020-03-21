#include "../include/yds_math.h"

#include <math.h>

ysGeneric ysMath::LoadScalar(float s) {
    return _mm_set_ps(s, s, s, s);
}

ysGeneric ysMath::LoadVector(float x, float y, float z, float w) {
    return _mm_set_ps(w, z, y, x);
}

ysGeneric ysMath::LoadVector(const ysVector4 &v) {
    return _mm_set_ps(v.w, v.z, v.y, v.x);
}

ysGeneric ysMath::LoadVector(const ysVector3 &v, float w) {
    return _mm_set_ps(w, v.z, v.y, v.x);
}

ysGeneric ysMath::LoadVector(const ysVector2 &v1) {
    return _mm_set_ps(0.0, 0.0, v1.y, v1.x);
}

ysGeneric ysMath::LoadVector(const ysVector2 &v1, const ysVector2 &v2) {
    return _mm_set_ps(v2.y, v2.x, v1.y, v1.x);
}

ysQuaternion ysMath::LoadQuaternion(float angle, const ysVector &axis) {
    float sinAngle = (float)sin(angle / 2.0f);
    float cosAngle = (float)cos(angle / 2.0f);

    ysVector newAxis = _mm_shuffle_ps(axis, axis, _MM_SHUFFLE(2, 1, 0, 3));
    newAxis = _mm_and_ps(newAxis, ysMath::Constants::MaskOffX);
    newAxis = _mm_mul_ps(newAxis, ysMath::LoadScalar(sinAngle));
    newAxis = _mm_or_ps(newAxis, ysMath::LoadVector(cosAngle));

    newAxis = ysMath::Normalize(newAxis);

    return newAxis;
}

ysVector4 ysMath::GetVector4(const ysVector &v) {
    ysVector4 r;
    r.x = v.m128_f32[0];
    r.y = v.m128_f32[1];
    r.z = v.m128_f32[2];
    r.w = v.m128_f32[3];

    return r;
}

ysVector3 ysMath::GetVector3(const ysVector &v) {
    ysVector3 r;
    r.x = v.m128_f32[0];
    r.y = v.m128_f32[1];
    r.z = v.m128_f32[2];

    return r;
}

ysVector2 ysMath::GetVector2(const ysVector &v) {
    ysVector2 r;
    r.x = v.m128_f32[0];
    r.y = v.m128_f32[1];

    return r;
}

float ysMath::GetScalar(const ysVector &v) {
    return v.m128_f32[0];
}

float ysMath::GetX(const ysVector &v) {
    return v.m128_f32[0];
}

float ysMath::GetY(const ysVector &v) {
    return v.m128_f32[1];
}

float ysMath::GetZ(const ysVector &v) {
    return v.m128_f32[2];
}

float ysMath::GetW(const ysVector &v) {
    return v.m128_f32[3];
}

float ysMath::GetQuatX(const ysQuaternion &v) {
    return v.m128_f32[1];
}

float ysMath::GetQuatY(const ysQuaternion &v) {
    return v.m128_f32[2];
}

float ysMath::GetQuatZ(const ysQuaternion &v) {
    return v.m128_f32[3];
}

float ysMath::GetQuatW(const ysQuaternion &v) {
    return v.m128_f32[0];
}

ysGeneric ysMath::Add(const ysGeneric &v1, const ysGeneric &v2) {
    return _mm_add_ps(v1, v2);
}

ysGeneric ysMath::Sub(const ysGeneric &v1, const ysGeneric &v2) {
    return _mm_sub_ps(v1, v2);
}

ysGeneric ysMath::Div(const ysGeneric &v1, const ysGeneric &v2) {
    return _mm_div_ps(v1, v2);
}

ysGeneric ysMath::Mul(const ysGeneric &v1, const ysGeneric &v2) {
    return _mm_mul_ps(v1, v2);
}

ysVector ysMath::Dot(const ysVector &v1, const ysVector &v2) {
    ysVector t0 = _mm_mul_ps(v1, v2);
    ysVector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));
    ysVector t2 = _mm_add_ps(t0, t1);
    ysVector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));
    ysVector dot = _mm_add_ps(t3, t2);
    return (dot);
}

ysVector ysMath::Dot3(const ysVector &v1, const ysVector &v2) {
    ysVector t0 = _mm_mul_ps(v1, v2);
    t0 = _mm_and_ps(t0, ysMath::Constants::MaskOffW);

    ysVector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));
    ysVector t2 = _mm_add_ps(t0, t1);
    ysVector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));
    ysVector dot = _mm_add_ps(t3, t2);
    return (dot);
}

ysVector ysMath::Cross(const ysVector &v1, const ysVector &v2) {
    // STOLEN FROM XNA MATH

    // y1, z1, x1, w1
    ysVector t1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));

    // z2, x2, y2, w2
    ysVector t2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 1, 0, 2));

    ysVector vResult = _mm_mul_ps(t1, t2);

    // z1, x1, y1, w1
    t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 0, 2, 1));

    // y2, z2, x2, w2
    t2 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(3, 1, 0, 2));

    // Perform the right operation
    t1 = _mm_mul_ps(t1, t2);

    // Subract the right from left, and return answer
    vResult = _mm_sub_ps(vResult, t1);

    // Set w to zero
    return _mm_and_ps(vResult, ysMath::Constants::MaskOffW);
}

ysVector ysMath::MagnitudeSquared3(const ysVector &v) {
    ysVector selfDot = ysMath::Dot3(v, v);

    return selfDot;
}

ysVector ysMath::Magnitude(const ysVector &v) {
    ysVector selfDot = ysMath::Dot(v, v);

    return _mm_sqrt_ps(selfDot);
}

ysVector ysMath::Normalize(const ysVector &v) {
    return ysMath::Div(v, ysMath::Magnitude(v));
}

ysVector ysMath::Negate(const ysVector &v) {
    return ysMath::Mul(v, ysMath::Constants::Negate);
}

ysVector ysMath::Negate3(const ysVector &v) {
    return ysMath::Mul(v, ysMath::Constants::Negate3);
}


ysVector ysMath::Mask(const ysVector &v, const ysVectorMask &mask) {
    return _mm_and_ps(v, mask.vector);
}

ysVector ysMath::Or(const ysVector &v1, const ysVector &v2) {
    return _mm_or_ps(v1, v2);
}

// Quaternion

ysQuaternion ysMath::QuatInvert(const ysQuaternion &q) {
    ysQuaternion nq = ysMath::LoadVector(1.0f, -1.0f, -1.0f, -1.0f);

    return ysMath::Mul(nq, q);
}

ysQuaternion ysMath::QuatMultiply(const ysQuaternion &q1, const ysQuaternion &q2) {
    ysGeneric w1 = _mm_replicate_x_ps(q1);
    ysGeneric x1 = _mm_replicate_y_ps(q1);
    ysGeneric y1 = _mm_replicate_z_ps(q1);
    ysGeneric z1 = _mm_replicate_w_ps(q1);

    ysGeneric m1 = q2;
    ysGeneric m2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 3, 0, 1)); // xwzy
    ysGeneric m3 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 0, 3, 2)); // yzwx
    ysGeneric m4 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 2, 3)); // zyxw

    ysGeneric sgn2 = _mm_set_ps(1, -1, 1, -1);
    ysGeneric sgn3 = _mm_set_ps(-1, 1, 1, -1);
    ysGeneric sgn4 = _mm_set_ps(1, 1, -1, -1);

    ysGeneric prod1 = _mm_mul_ps(w1, m1);
    ysGeneric prod2 = _mm_mul_ps(_mm_mul_ps(x1, sgn2), m2);
    ysGeneric prod3 = _mm_mul_ps(_mm_mul_ps(y1, sgn3), m3);
    ysGeneric prod4 = _mm_mul_ps(_mm_mul_ps(z1, sgn4), m4);

    ysGeneric result = _mm_add_ps(_mm_add_ps(prod1, prod2), _mm_add_ps(prod3, prod4));

    return (ysQuaternion)result;
}

ysQuaternion ysMath::QuatAddScaled(const ysQuaternion &q, const ysVector &vec, float scale) {
    ysGeneric n = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 1, 0, 3));
    n = _mm_and_ps(n, ysMath::Constants::MaskOffX);

    n = _mm_mul_ps(n, ysMath::LoadScalar(scale));

    ysQuaternion q2 = n;
    ysQuaternion m1 = ysMath::QuatMultiply(q2, q);

    ysQuaternion ret = _mm_add_ps(q, _mm_mul_ps(m1, ysMath::Constants::Half));

    return ysMath::Normalize(ret);
}

// Matrices

ysMatrix ysMath::LoadIdentity() {
    ysMatrix r;
    r.rows[0] = ysMath::Constants::IdentityRow1;
    r.rows[1] = ysMath::Constants::IdentityRow2;
    r.rows[2] = ysMath::Constants::IdentityRow3;
    r.rows[3] = ysMath::Constants::IdentityRow4;

    return r;
}

ysMatrix ysMath::LoadMatrix(const ysVector &r1, const ysVector &r2, const ysVector &r3, const ysVector &r4) {
    ysMatrix r;
    r.rows[0] = r1;
    r.rows[1] = r2;
    r.rows[2] = r3;
    r.rows[3] = r4;

    return r;
}

ysMatrix ysMath::LoadMatrix(const ysQuaternion &quat) {
    // 21 instruction implementation

    ysGeneric q = quat;
    ysGeneric nq = _mm_sub_ps(ysMath::Constants::Zero, q);
    ysGeneric qq = _mm_add_ps(q, q);
    ysGeneric q2 = _mm_mul_ps(qq, q);

    ysGeneric xyxx = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 2, 1));
    ysGeneric yzzy = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(2, 3, 3, 2));
    ysGeneric zxyz = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(3, 2, 1, 3));
    ysGeneric wwww = _mm_shuffle_ps(q, nq, _MM_SHUFFLE(0, 0, 0, 0));

    ysGeneric i1 = _mm_mul_ps(xyxx, yzzy); // [2xy, 2yz, 2xz, 2xy]
    ysGeneric i2 = _mm_mul_ps(zxyz, wwww); // [2zw, 2xw, -2yw, -2zw ]
    ysGeneric calc1 = _mm_add_ps(i1, i2);

    // Stage 2

    ysGeneric y2_x2_x2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 1, 2));
    ysGeneric z2_z2_y2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 2, 3, 3));

    ysGeneric calc2 = _mm_sub_ps(ysMath::Constants::One, _mm_add_ps(y2_x2_x2_w2, z2_z2_y2_w2));
    calc2 = _mm_and_ps(calc2, ysMath::Constants::MaskOffW.vector);

    // Stage 3

    ysGeneric calc3 = _mm_sub_ps(i1, i2);

    // Assembly

    // 2xy - 2zw -> 3
    // 2xz - 2yw -> 2
    // 2xy + 2zw -> 0
    // 2yz + 2xw -> 1

    ysGeneric asm1 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(2, 0, 3, 0));
    asm1 = _mm_shuffle_ps(asm1, asm1, _MM_SHUFFLE(1, 3, 2, 0));

    ysGeneric asm2 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(1, 3, 3, 1));
    asm2 = _mm_shuffle_ps(asm2, asm2, _MM_SHUFFLE(1, 3, 0, 2));

    ysGeneric asm3 = _mm_shuffle_ps(calc3, calc2, _MM_SHUFFLE(3, 2, 1, 2));
    // No need to shuffle this one

    ysMatrix ret = ysMath::LoadMatrix(asm1, asm2, asm3, ysMath::Constants::IdentityRow4);

    return ret;
}

ysMatrix ysMath::LoadMatrix(const ysQuaternion &quat, const ysVector &origin) {
    ysGeneric q = quat;
    ysGeneric nq = _mm_sub_ps(ysMath::Constants::Zero, q);
    ysGeneric qq = _mm_add_ps(q, q);
    ysGeneric q2 = _mm_mul_ps(qq, q);

    ysGeneric xxxy = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 1, 1, 1));
    ysGeneric zyyz = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(3, 2, 2, 3));
    ysGeneric yzzx = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(1, 3, 3, 2));
    ysGeneric wwww = _mm_shuffle_ps(q, nq, _MM_SHUFFLE(0, 0, 0, 0));

    ysGeneric i1 = _mm_mul_ps(xxxy, zyyz);	// [2xz, 2xy, 2xy, 2yz]
    ysGeneric i2 = _mm_mul_ps(yzzx, wwww); // [2yw, 2zw, -2zw, -2xw]
    ysGeneric calc1 = _mm_add_ps(i1, i2);	// [2xz - 2yw, 2xy + 2zy, 2xy - 2zy, 2yz - 2xw]

    // Stage 2

    ysGeneric y2_x2_x2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 1, 2));
    ysGeneric z2_z2_y2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 2, 3, 3));

    ysGeneric calc2 = _mm_sub_ps(ysMath::Constants::One, _mm_add_ps(y2_x2_x2_w2, z2_z2_y2_w2));
    calc2 = _mm_and_ps(calc2, ysMath::Constants::MaskOffW.vector);

    // Stage 3

    ysGeneric calc3 = _mm_sub_ps(i1, i2);	// [2xz + 2yw, 2xy - 2zy, 2xy + 2zy, 2yz + 2xw]

    // Assembly

    // 2xz + 2yw -> 0
    // 2xy + 2zw -> 1
    // 2xy - 2zw -> 2
    // 2yz - 2xw -> 3

    ysGeneric asm1 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(0, 2, 3, 0));
    asm1 = _mm_shuffle_ps(asm1, asm1, _MM_SHUFFLE(1, 3, 2, 0));

    ysGeneric asm2 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(3, 1, 3, 1));
    asm2 = _mm_shuffle_ps(asm2, asm2, _MM_SHUFFLE(1, 3, 0, 2));

    ysGeneric asm3 = _mm_shuffle_ps(calc3, calc2, _MM_SHUFFLE(3, 2, 3, 0));
    // No need to shuffle this one

    ysGeneric asm4 = _mm_and_ps(origin, ysMath::Constants::MaskOffW.vector);
    asm4 = _mm_add_ps(asm4, ysMath::Constants::IdentityRow4);

    ysMatrix ret = ysMath::Transpose(ysMath::LoadMatrix(asm1, asm2, asm3, asm4));

    return ret;
}

void ysMath::LoadMatrix(const ysQuaternion &quat, const ysVector &origin, ysMatrix *full, ysMatrix *orientation) {
    ysGeneric q = quat;
    ysGeneric nq = _mm_sub_ps(ysMath::Constants::Zero, q);
    ysGeneric qq = _mm_add_ps(q, q);
    ysGeneric q2 = _mm_mul_ps(qq, q);

    ysGeneric xxxy = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 1, 1, 1));
    ysGeneric zyyz = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(3, 2, 2, 3));
    ysGeneric yzzx = _mm_shuffle_ps(qq, qq, _MM_SHUFFLE(1, 3, 3, 2));
    ysGeneric wwww = _mm_shuffle_ps(q, nq, _MM_SHUFFLE(0, 0, 0, 0));

    ysGeneric i1 = _mm_mul_ps(xxxy, zyyz); // [2xz, 2xy, 2xy, 2yz]
    ysGeneric i2 = _mm_mul_ps(yzzx, wwww); // [2yw, 2zw, -2zw, -2xw]
    ysGeneric calc1 = _mm_add_ps(i1, i2);

    // Stage 2

    ysGeneric y2_x2_x2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 1, 1, 2));
    ysGeneric z2_z2_y2_w2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 2, 3, 3));

    ysGeneric calc2 = _mm_sub_ps(ysMath::Constants::One, _mm_add_ps(y2_x2_x2_w2, z2_z2_y2_w2));
    calc2 = _mm_and_ps(calc2, ysMath::Constants::MaskOffW.vector);

    // Stage 3

    ysGeneric calc3 = _mm_sub_ps(i1, i2);

    // Assembly

    // 2xz + 2yw -> 0
    // 2xy + 2zw -> 1
    // 2xy - 2zw -> 2
    // 2yz - 2xw -> 3

    ysGeneric asm1 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(0, 2, 3, 0));
    asm1 = _mm_shuffle_ps(asm1, asm1, _MM_SHUFFLE(1, 3, 2, 0));

    ysGeneric asm2 = _mm_shuffle_ps(calc2, calc1, _MM_SHUFFLE(3, 1, 3, 1));
    asm2 = _mm_shuffle_ps(asm2, asm2, _MM_SHUFFLE(1, 3, 0, 2));

    ysGeneric asm3 = _mm_shuffle_ps(calc3, calc2, _MM_SHUFFLE(3, 2, 3, 0));
    // No need to shuffle this one

    *orientation = ysMath::Transpose(ysMath::LoadMatrix(asm1, asm2, asm3, ysMath::Constants::IdentityRow4));

    ysGeneric asm4 = _mm_and_ps(origin, ysMath::Constants::MaskOffW.vector);
    asm4 = _mm_add_ps(asm4, ysMath::Constants::IdentityRow4);

    *full = ysMath::Transpose(ysMath::LoadMatrix(asm1, asm2, asm3, asm4));
}

ysMatrix ysMath::Transpose(const ysMatrix &m) {
    ysMatrix r = m;

    _MM_TRANSPOSE4_PS(r.rows[0], r.rows[1], r.rows[2], r.rows[3]);

    return r;
}

ysMatrix ysMath::OrthogonalInverse(const ysMatrix &m) {
    ysMatrix r = m;

    _MM_TRANSPOSE4_PS(r.rows[0], r.rows[1], r.rows[2], r.rows[3]);

    // RTv
    // (Tinv)(Rinv)v

    ysMatrix r_inv = LoadMatrix(r.rows[0], r.rows[1], r.rows[2], ysMath::Constants::IdentityRow4);
    ysMatrix t_inv = TranslationTransform(ysMath::Negate3(r.rows[3]));

    r = MatMult(r_inv, t_inv);

    return r;
}

ysMatrix44 ysMath::GetMatrix44(const ysMatrix &m) {
    ysMatrix44 r;
    r.rows[0] = ysMath::GetVector4(m.rows[0]);
    r.rows[1] = ysMath::GetVector4(m.rows[1]);
    r.rows[2] = ysMath::GetVector4(m.rows[2]);
    r.rows[3] = ysMath::GetVector4(m.rows[3]);

    return r;
}

ysMatrix33 ysMath::GetMatrix33(const ysMatrix &m) {
    ysMatrix33 r;
    r.rows[0] = ysMath::GetVector3(m.rows[0]);
    r.rows[1] = ysMath::GetVector3(m.rows[1]);
    r.rows[2] = ysMath::GetVector3(m.rows[2]);

    return r;
}

ysVector ysMath::ExtendVector(const ysVector &v) {
    ysVector ret = v;
    ret = ysMath::Mask(v, ysMath::Constants::MaskOffW);
    ret = _mm_or_ps(ret, ysMath::Constants::IdentityRow4);
    return ret;
}

ysVector ysMath::MatMult(const ysMatrix &m, const ysVector &v) {
    ysMatrix t = m;
    _MM_TRANSPOSE4_PS(t.rows[0], t.rows[1], t.rows[2], t.rows[3]);

    //ysVector exV = ExtendVector(v);

    ysVector r;
    r = _mm_mul_ps(_mm_replicate_x_ps(v), t.rows[0]);
    r = _mm_madd_ps(_mm_replicate_y_ps(v), t.rows[1], r);
    r = _mm_madd_ps(_mm_replicate_z_ps(v), t.rows[2], r);
    r = _mm_madd_ps(_mm_replicate_w_ps(v), t.rows[3], r);

    return r;
}

ysMatrix ysMath::MatMult(const ysMatrix &m1, const ysMatrix &m2) {
    ysMatrix r;

    for (int i = 0; i < 4; i++) {
        r.rows[i] = _mm_mul_ps(_mm_replicate_x_ps(m1.rows[i]), m2.rows[0]);
        r.rows[i] = _mm_madd_ps(_mm_replicate_y_ps(m1.rows[i]), m2.rows[1], r.rows[i]);
        r.rows[i] = _mm_madd_ps(_mm_replicate_z_ps(m1.rows[i]), m2.rows[2], r.rows[i]);
        r.rows[i] = _mm_madd_ps(_mm_replicate_w_ps(m1.rows[i]), m2.rows[3], r.rows[i]);
    }

    return r;
}

ysMatrix ysMath::FrustrumPerspective(float fovy, float aspect, float near, float far) {
    float sinfov = ::sinf(fovy / 2.0f);
    float cosfov = ::cosf(fovy / 2.0f);

    float height = cosfov / sinfov;
    float width = height / aspect;

    ysVector row1 = ysMath::LoadVector(width, 0, 0, 0);
    ysVector row2 = ysMath::LoadVector(0, height, 0, 0);
    ysVector row3 = ysMath::LoadVector(0, 0, (far) / (far - near), 1.0);
    ysVector row4 = ysMath::LoadVector(0, 0, -(far * near) / (far - near), 0);

    return LoadMatrix(row1, row2, row3, row4);
}

ysMatrix ysMath::OrthographicProjection(float width, float height, float near, float far) {
    float fRange = 1.0f / (far - near);

    ysVector row1 = ysMath::LoadVector(2.0f / width, 0.0f, 0.0f, 0.0f);
    ysVector row2 = ysMath::LoadVector(0.0f, 2.0f / height, 0.0f, 0.0f);
    ysVector row3 = ysMath::LoadVector(0.0f, 0.0f, 2 * fRange, 0.0f);
    ysVector row4 = ysMath::LoadVector(0.0f, 0.0f, -fRange * near, 1.0f);

    return LoadMatrix(row1, row2, row3, row4);
}

ysMatrix ysMath::CameraTarget(const ysVector &eye, const ysVector &target, const ysVector &up) {
    ysVector R2 = ysMath::Sub(target, eye);
    R2 = ysMath::Normalize(R2);

    ysVector R0 = ysMath::Cross(R2, up);
    R0 = ysMath::Normalize(R0);

    ysVector R1 = ysMath::Cross(R0, R2);

    ysVector negEyePos = ysMath::Negate(eye);

    ysVector D0 = ysMath::Dot(R0, negEyePos);
    ysVector D1 = ysMath::Dot(R1, negEyePos);
    ysVector D2 = ysMath::Dot(R2, negEyePos);

    R0 = ysMath::Mask(R0, ysMath::Constants::MaskOffW);
    R1 = ysMath::Mask(R1, ysMath::Constants::MaskOffW);
    R2 = ysMath::Mask(R2, ysMath::Constants::MaskOffW);

    D0 = ysMath::Mask(D0, ysMath::Constants::MaskKeepW);
    D1 = ysMath::Mask(D1, ysMath::Constants::MaskKeepW);
    D2 = ysMath::Mask(D2, ysMath::Constants::MaskKeepW);

    D0 = ysMath::Or(R0, D0);
    D1 = ysMath::Or(R1, D1);
    D2 = ysMath::Or(R2, D2);

    ysMatrix r;
    r.rows[0] = D0;
    r.rows[1] = D1;
    r.rows[2] = D2;
    r.rows[3] = ysMath::Constants::IdentityRow4;

    r = ysMath::Transpose(r);

    return r;
}

ysMatrix ysMath::TranslationTransform(const ysVector &translation) {
    ysMatrix r;
    r.rows[0] = ysMath::Constants::IdentityRow1;
    r.rows[1] = ysMath::Constants::IdentityRow2;
    r.rows[2] = ysMath::Constants::IdentityRow3;

    ysVector trans = ysMath::Mask(translation, ysMath::Constants::MaskOffW);
    r.rows[3] = ysMath::Or(trans, ysMath::Constants::IdentityRow4);

    r = ysMath::Transpose(r);

    return r;
}

ysMatrix ysMath::ScaleTransform(const ysVector &scale) {
    ysMatrix r;
    r.rows[0] = ysMath::Mask(scale, ysMath::Constants::MaskKeepX);
    r.rows[1] = ysMath::Mask(scale, ysMath::Constants::MaskKeepY);
    r.rows[2] = ysMath::Mask(scale, ysMath::Constants::MaskKeepZ);
    r.rows[3] = ysMath::Constants::IdentityRow4;

    return r;
}

ysMatrix ysMath::RotationTransform(const ysVector &axis, float angle) {
    // TEMP
    // BAD IMPLEMENTATION
    ysVector naxis = ysMath::Normalize(axis);

    float ux = ysMath::GetX(naxis);
    float uy = ysMath::GetY(naxis);
    float uz = ysMath::GetZ(naxis);

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    ysMatrix r;
    r.rows[0] = ysMath::LoadVector(cos_a + ux * ux * (1 - cos_a), ux * uy * (1 - cos_a) - uz * sin_a, ux * uz * (1 - cos_a) + uy * sin_a);
    r.rows[1] = ysMath::LoadVector(uy * ux * (1 - cos_a) + uz * sin_a, cos_a + uy * uy * (1 - cos_a), uy * uz * (1 - cos_a) - ux * sin_a);
    r.rows[2] = ysMath::LoadVector(uz * ux * (1 - cos_a) - uy * sin_a, uz * uy * (1 - cos_a) + ux * sin_a, cos_a + uz * uz * (1 - cos_a));
    r.rows[3] = ysMath::Constants::IdentityRow4;

    return r;
}

ysVector ysMath::GetTranslationPart(const ysMatrix &mat) {
    ysMatrix r = ysMath::Transpose(mat);

    return r.rows[3];
}

ysVector ysMath::ComponentMax(const ysVector &a, const ysVector &b) {
    ysVector result = _mm_max_ps(a, b);
    return result;
}

ysVector ysMath::ComponentMin(const ysVector &a, const ysVector &b) {
    ysVector result = _mm_min_ps(a, b);
    return result;
}
