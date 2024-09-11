#ifndef YDS_MATH_H
#define YDS_MATH_H

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
//    #include <arm_neon.h>
    #include "sse2neon.h"
#elif defined(_WIN64)
    #include <xmmintrin.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Extra Definitions

#define _mm_replicate_x_ps(v) \
    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(0, 0, 0, 0))

#define _mm_replicate_y_ps(v) \
    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(1, 1, 1, 1))

#define _mm_replicate_z_ps(v) \
    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(2, 2, 2, 2))

#define _mm_replicate_w_ps(v) \
    _mm_shuffle_ps((v), (v), _MM_SHUFFLE(3, 3, 3, 3))

#define _mm_madd_ps(a, b, c) \
    _mm_add_ps(_mm_mul_ps((a), (b)), (c));

// compiler-specific keyword which suggests that a function should be inlined
#if defined(__APPLE__) && defined(__MACH__)
    #define forceInline __attribute__((always_inline))
#elif defined(_WIN64)
    #define forceInline __forceinline
#endif

// Main Arithmetic Data Types
typedef __m128 ysVector;
typedef __m128 ysQuaternion;
typedef __m128 ysGeneric;

struct ysVectorMask {
    union {
        struct {
            int mask[4];
        };

        ysVector vector;
    };

    operator ysVector() const { return vector; }
};

struct ysMatrix {
    ysVector rows[4];
};


// Storage Data Types

struct ysVector2 {
    ysVector2() : x(0.0f), y(0.0f) { /* void */ }
    ysVector2(float x, float y) : x(x), y(y) { /* void */ }

    union {
        struct {
            float x, y;
        };

        float vec[2];
    };
};

struct ysVector3 {
    ysVector3() : x(0), y(0), z(0) { /* void */ }
    ysVector3(float x, float y, float z) : x(x), y(y), z(z) { /* void */ }
    ysVector3(const ysVector2 &v) : x(v.x), y(v.y), z(0) { /* void */ }

    union {
        struct {
            float x, y, z;
        };

        float vec[3];
    };
};

struct ysVector4 {
    inline ysVector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { /* void */ }
    inline ysVector4(float x, float y, float z, float w = 0.0f) : x(x), y(y), z(z), w(w) { /* void */ }
    inline ysVector4(const ysVector3 &v) : x(v.x), y(v.y), z(v.z), w(0.0f) { /* void */ }
    inline ysVector4(const ysVector &v);

    void Set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
    void Scale(float s) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; }

    union {
        struct {
            float x, y, z, w;
        };

        float vec[4];
    };
};

struct ysMatrix44 {
    ysMatrix44() { ::memset((void *)data, 0x0, sizeof(data)); }
    ~ysMatrix44() { /* void */  }

    union {
        struct {
            ysVector4 rows[4];
        };

        float data[4][4];
    };
};

struct ysMatrix33 {
    ysMatrix33() { ::memset((void *)data, 0x0, sizeof(data)); }
    ~ysMatrix33() { /* void */ }

    union {
        struct {
            ysVector3 rows[3];
        };

        float data[3][3];
    };
};

#if defined(_WIN64)
    #define YS_MATH_CONST extern const __declspec(selectany)
#endif

namespace ysMath {

    namespace Constants {

        // ----------------------------------------------------
        // Constants
        // ----------------------------------------------------

        // Masks
    #if defined(__APPLE__) && defined(__MACH__)
    
        ysVectorMask MaskOffW = { (int)0xFFFFFFFF,(int)0xFFFFFFFF,(int)0xFFFFFFFF, (int)0x00000000 };
        ysVectorMask MaskOffZ = { (int)0xFFFFFFFF, (int)0xFFFFFFFF, (int)0x00000000, (int)0xFFFFFFFF };
        ysVectorMask MaskOffY = { (int)0xFFFFFFFF, (int)0x00000000, (int)0xFFFFFFFF, (int)0xFFFFFFFF };
        ysVectorMask MaskOffX = { (int)0x00000000, (int)0xFFFFFFFF, (int)0xFFFFFFFF, (int)0xFFFFFFFF };

        ysVectorMask MaskKeepW = { (int)0x00000000, (int)0x00000000, (int)0x00000000, (int)0xFFFFFFFF };
        ysVectorMask MaskKeepZ = { (int)0x00000000, (int)0x00000000, (int)0xFFFFFFFF, (int)0x00000000 };
        ysVectorMask MaskKeepY = { (int)0x00000000, (int)0xFFFFFFFF, (int)0x00000000, (int)0x00000000 };
        ysVectorMask MaskKeepX = { (int)0xFFFFFFFF, (int)0x00000000, (int)0x00000000, (int)0x00000000 };

    // Axes
        ysVector XAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
        ysVector YAxis = { 0.0f, 1.0f, 0.0f, 0.0f };
        ysVector ZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };

    // Constants
        ysVector IdentityRow1 = { 1.0f, 0.0f, 0.0f, 0.0f };
        ysVector IdentityRow2 = { 0.0f, 1.0f, 0.0f, 0.0f };
        ysVector IdentityRow3 = { 0.0f, 0.0f, 1.0f, 0.0f };
        ysVector IdentityRow4 = { 0.0f, 0.0f, 0.0f, 1.0f };

        ysVector Negate = { -1.0f, -1.0f, -1.0f, -1.0f };
        ysVector Negate3 = { -1.0f, -1.0f, -1.0f, 1.0f };
        ysVector QuatInvert = { 1.0f, -1.0f, -1.0f, -1.0f };
        ysVector One = { 1.0f, 1.0f, 1.0f, 1.0f };
        ysVector Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
        ysVector Zero3 = { 0.0f, 0.0f, 0.0f, 1.0f };
        ysVector Half = { 0.5f, 0.5f, 0.5f, 0.5f };
        ysVector Double = { 2.0f, 2.0f, 2.0f, 2.0f };

        ysMatrix Identity = {
        IdentityRow1,
        IdentityRow2,
        IdentityRow3,
        IdentityRow4 };

    // Numeral Constants
        float PI = 3.141592654f;
        float TWO_PI = 6.2831853071795864769252866f;
        float SQRT_2 = 1.41421356237f;

    // Quaternions
        ysQuaternion QuatIdentity = { 1.0f, 0.0f, 0.0f, 0.0f };
    
    #elif defined(_WIN64)
    
        YS_MATH_CONST ysVectorMask MaskOffW = { (int)0xFFFFFFFF,(int)0xFFFFFFFF,(int)0xFFFFFFFF, (int)0x00000000 };
        YS_MATH_CONST ysVectorMask MaskOffZ = { (int)0xFFFFFFFF, (int)0xFFFFFFFF, (int)0x00000000, (int)0xFFFFFFFF };
        YS_MATH_CONST ysVectorMask MaskOffY = { (int)0xFFFFFFFF, (int)0x00000000, (int)0xFFFFFFFF, (int)0xFFFFFFFF };
        YS_MATH_CONST ysVectorMask MaskOffX = { (int)0x00000000, (int)0xFFFFFFFF, (int)0xFFFFFFFF, (int)0xFFFFFFFF };

        YS_MATH_CONST ysVectorMask MaskKeepW = { (int)0x00000000, (int)0x00000000, (int)0x00000000, (int)0xFFFFFFFF };
        YS_MATH_CONST ysVectorMask MaskKeepZ = { (int)0x00000000, (int)0x00000000, (int)0xFFFFFFFF, (int)0x00000000 };
        YS_MATH_CONST ysVectorMask MaskKeepY = { (int)0x00000000, (int)0xFFFFFFFF, (int)0x00000000, (int)0x00000000 };
        YS_MATH_CONST ysVectorMask MaskKeepX = { (int)0xFFFFFFFF, (int)0x00000000, (int)0x00000000, (int)0x00000000 };

        // Axes
        YS_MATH_CONST ysVector XAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
        YS_MATH_CONST ysVector YAxis = { 0.0f, 1.0f, 0.0f, 0.0f };
        YS_MATH_CONST ysVector ZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };

        // Constants
        YS_MATH_CONST ysVector IdentityRow1 = { 1.0f, 0.0f, 0.0f, 0.0f };
        YS_MATH_CONST ysVector IdentityRow2 = { 0.0f, 1.0f, 0.0f, 0.0f };
        YS_MATH_CONST ysVector IdentityRow3 = { 0.0f, 0.0f, 1.0f, 0.0f };
        YS_MATH_CONST ysVector IdentityRow4 = { 0.0f, 0.0f, 0.0f, 1.0f };

        YS_MATH_CONST ysVector Negate = { -1.0f, -1.0f, -1.0f, -1.0f };
        YS_MATH_CONST ysVector Negate3 = { -1.0f, -1.0f, -1.0f, 1.0f };
        YS_MATH_CONST ysVector QuatInvert = { 1.0f, -1.0f, -1.0f, -1.0f };
        YS_MATH_CONST ysVector One = { 1.0f, 1.0f, 1.0f, 1.0f };
        YS_MATH_CONST ysVector Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
        YS_MATH_CONST ysVector Zero3 = { 0.0f, 0.0f, 0.0f, 1.0f };
        YS_MATH_CONST ysVector Half = { 0.5f, 0.5f, 0.5f, 0.5f };
        YS_MATH_CONST ysVector Double = { 2.0f, 2.0f, 2.0f, 2.0f };

        YS_MATH_CONST ysMatrix Identity = { 
            IdentityRow1, 
            IdentityRow2, 
            IdentityRow3, 
            IdentityRow4 };

        // Numeral Constants
        YS_MATH_CONST float PI = 3.141592654f;
        YS_MATH_CONST float TWO_PI = 6.2831853071795864769252866f;
        YS_MATH_CONST float SQRT_2 = 1.41421356237f;

        // Quaternions
        YS_MATH_CONST ysQuaternion QuatIdentity = { 1.0f, 0.0f, 0.0f, 0.0f };
    #endif

    } /* namespace Constants */

    // ----------------------------------------------------
    // Math Functions
    // ----------------------------------------------------

    // Math functions
    ysVector UniformRandom4(float range = (float)1.0);
    float UniformRandom(float range = (float)1.0);
    int UniformRandomInt(int range);

    // Vector/General Quaternion
    forceInline ysGeneric LoadScalar(float s) {
        return _mm_set_ps(s, s, s, s);
    }

    forceInline ysGeneric LoadVector(float x = 0.0f, float y = 0.0f, float z = 0.0f,
        float w = 0.0f) {
        return _mm_set_ps(w, z, y, x);
    }

    ysGeneric LoadVector(const ysVector4 &v);
    ysGeneric LoadVector(const ysVector3 &v, float w = 0.0f);
    ysGeneric LoadVector(const ysVector2 &v1);
    ysGeneric LoadVector(const ysVector2 &v1, const ysVector2 &v2);
    ysGeneric Lerp(const ysGeneric &a, const ysGeneric &b, float s);
    ysQuaternion LoadQuaternion(float angle, const ysVector &axis);

    inline ysVector4 GetVector4(const ysVector &v) {
        ysVector4 r;
    
    #if defined(__APPLE__) && defined(__MACH__)
        // Access individual elements using NEON intrinsics
        r.x = vgetq_lane_f32(v, 0);
        r.y = vgetq_lane_f32(v, 1);
        r.z = vgetq_lane_f32(v, 2);
        r.w = vgetq_lane_f32(v, 3);
    #elif defined(_WIN64)
        r.x = v.m128_f32[0];
        r.y = v.m128_f32[1];
        r.z = v.m128_f32[2];
        r.w = v.m128_f32[3];
    #endif

        return r;
    }

    inline ysVector3 GetVector3(const ysVector &v) {
        ysVector3 r;
        
    #if defined(__APPLE__) && defined(__MACH__)
        r.x = vgetq_lane_f32(v, 0);
        r.y = vgetq_lane_f32(v, 1);
        r.z = vgetq_lane_f32(v, 2);
    #elif defined(_WIN64)
        r.x = v.m128_f32[0];
        r.y = v.m128_f32[1];
        r.z = v.m128_f32[2];
    #endif

        return r;
    }

    inline ysVector2 GetVector2(const ysVector &v) {
        ysVector2 r;
        
    #if defined(__APPLE__) && defined(__MACH__)
        r.x = vgetq_lane_f32(v, 0);
        r.y = vgetq_lane_f32(v, 1);
    #elif defined(_WIN64)
        r.x = v.m128_f32[0];
        r.y = v.m128_f32[1];
    #endif

        return r;
    }

    forceInline float GetScalar(const ysVector &v) {
    #if defined(__APPLE__) && defined(__MACH__)
        return vgetq_lane_f32(v, 0);
    #elif defined(_WIN64)
        return v.m128_f32[0];
    #endif
    }

    forceInline float GetX(const ysVector &v) {
    #if defined(__APPLE__) && defined(__MACH__)
        return vgetq_lane_f32(v, 0);
    #elif defined(_WIN64)
        return v.m128_f32[0];
    #endif
    }

    forceInline float GetY(const ysVector &v) {
    #if defined(__APPLE__) && defined(__MACH__)
        return vgetq_lane_f32(v, 1);
    #elif defined(_WIN64)
        return v.m128_f32[1];
    #endif
    }

    forceInline float GetZ(const ysVector &v) {
    #if defined(__APPLE__) && defined(__MACH__)
    return vgetq_lane_f32(v, 2);
    #elif defined(_WIN64)
        return v.m128_f32[2];
    #endif
    }

    forceInline float GetW(const ysVector &v) {
    #if defined(__APPLE__) && defined(__MACH__)
        return vgetq_lane_f32(v, 3);
    #elif defined(_WIN64)
        return v.m128_f32[3];
    #endif
    }

    float GetQuatX(const ysQuaternion &v);
    float GetQuatY(const ysQuaternion &v);
    float GetQuatZ(const ysQuaternion &v);
    float GetQuatW(const ysQuaternion &v);

    forceInline ysGeneric Add(const ysGeneric &v1, const ysGeneric &v2) {
        return _mm_add_ps(v1, v2);
    }

    forceInline ysGeneric Sub(const ysGeneric &v1, const ysGeneric &v2) {
        return _mm_sub_ps(v1, v2);
    }

    forceInline ysGeneric Mul(const ysGeneric &v1, const ysGeneric &v2) {
        return _mm_mul_ps(v1, v2);
    }

    forceInline ysGeneric Div(const ysGeneric &v1, const ysGeneric &v2) {
        return _mm_div_ps(v1, v2);
    }

    forceInline ysGeneric Sqrt(const ysGeneric &v) {
        return _mm_sqrt_ps(v);
    }

    forceInline ysVector Dot(const ysVector &v1, const ysVector &v2) {
        ysVector t0 = _mm_mul_ps(v1, v2);
        ysVector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));
        ysVector t2 = _mm_add_ps(t0, t1);
        ysVector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));
        ysVector dot = _mm_add_ps(t3, t2);
        return (dot);
    }

    ysVector Dot3(const ysVector &v1, const ysVector &v2);
    ysVector Cross(const ysVector &v1, const ysVector &v2);

    forceInline ysVector MagnitudeSquared3(const ysVector &v) {
        ysVector selfDot = ysMath::Dot3(v, v);

        return selfDot;
    }

    forceInline ysVector Magnitude(const ysVector &v) {
        ysVector selfDot = ysMath::Dot(v, v);

        return _mm_sqrt_ps(selfDot);
    }

    forceInline ysVector Normalize(const ysVector &v) {
        return ysMath::Div(v, ysMath::Magnitude(v));
    }

    forceInline ysVector Negate(const ysVector &v) {
        return ysMath::Mul(v, ysMath::Constants::Negate);
    }

    ysVector Negate3(const ysVector &v);
    ysVector Abs(const ysVector &a);

    ysVector Mask(const ysVector &v, const ysVectorMask &mask);
    ysVector Or(const ysVector &v1, const ysVector &v2);

    forceInline ysVector GreaterThan(const ysVector &a, const ysVector &b) {
        const ysVector cmp_mask = _mm_cmpge_ps(a, b);
        return _mm_and_ps(cmp_mask, Constants::One);
    }

    forceInline ysVector LessThan(const ysVector &a, const ysVector &b) {
        const ysVector cmp_mask = _mm_cmple_ps(a, b);
        return _mm_and_ps(cmp_mask, Constants::One);
    }

    // Quaternion
    ysQuaternion QuatInvert(const ysQuaternion &q);
    ysQuaternion QuatMultiply(const ysQuaternion &q1, const ysQuaternion &q2);
    ysQuaternion QuatTransform(const ysQuaternion &q, const ysVector &v);
    ysQuaternion QuatTransformInverse(const ysQuaternion &q, const ysVector &v);
    ysQuaternion QuatAddScaled(const ysQuaternion &q, const ysVector &vec, float scale);

    // Matrices
    ysMatrix LoadIdentity();
    ysMatrix LoadMatrix(const ysVector &r1, const ysVector &r2, const ysVector &r3, const ysVector &r4);
    ysMatrix LoadMatrix(const ysQuaternion &quat);
    ysMatrix LoadMatrix(const ysQuaternion &quat, const ysVector &origin);
    void LoadMatrix(const ysQuaternion &quat, const ysVector &origin, ysMatrix *full, ysMatrix *orientation);

    ysMatrix Transpose(const ysMatrix &m);
    ysVector Det3x3(const ysMatrix &m);
    ysMatrix OrthogonalInverse(const ysMatrix &m);
    ysMatrix Inverse3x3(const ysMatrix &m);
    ysMatrix Negate4x4(const ysMatrix &m);
    ysMatrix Negate3x3(const ysMatrix &m);

    ysMatrix SkewSymmetric(const ysVector &v);

    ysMatrix44 GetMatrix44(const ysMatrix &m);
    ysMatrix33 GetMatrix33(const ysMatrix &m);

    ysVector ExtendVector(const ysVector &v);
    ysVector MatMult(const ysMatrix &m, const ysVector &v);
    ysMatrix MatMult(const ysMatrix &m1, const ysMatrix &m2);
    ysMatrix MatAdd(const ysMatrix &m1, const ysMatrix &m2);
    ysMatrix MatConvert3x3(const ysMatrix &m);
    ysMatrix MatNormalize(const ysMatrix &m);

    // Common Matrix Calculations
    ysMatrix FrustrumPerspective(float fovy, float aspect, float near, float far);
    ysMatrix OrthographicProjection(float width, float height, float near, float far);
    ysMatrix CameraTarget(const ysVector &eye, const ysVector &target, const ysVector &up);

    ysMatrix TranslationTransform(const ysVector &translation);
    ysMatrix ScaleTransform(const ysVector &scale);
    ysMatrix RotationTransform(const ysVector &axis, float angle);

    ysVector GetTranslationPart(const ysMatrix &mat);

    ysVector ComponentMax(const ysVector &a, const ysVector &b);
    ysVector ComponentMin(const ysVector &a, const ysVector &b);
    ysVector Clamp(const ysVector &a, const ysVector &r_min, const ysVector &r_max);

    ysVector MaxComponent(const ysVector &v);

    bool IsValid(const ysVector &v);

} /* namespace ysMath */

ysVector4::ysVector4(const ysVector &v) { *this = ysMath::GetVector4(v); }

#endif /* YDS_MATH_H */
