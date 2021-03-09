#include "../include/yds_color.h"

#include <cmath>

float ysColor::srgbGamma(float u) {
    constexpr float MinSrgbPower = 0.0031308f;

    if (u < MinSrgbPower) {
        return 12.92f * u;
    }
    else {
        return 1.055f * ::pow(u, 1 / 2.4f) - 0.055f;
    }
}

float ysColor::inverseGammaSrgb(float u) {
    static constexpr float MinSrgbPower = 0.04045f;

    if (u < MinSrgbPower) {
        return u / 12.92f;
    }
    else {
        return ::pow((u + 0.055f) / 1.055f, 2.4f);
    }
}

ysVector ysColor::srgbToLinear(float r, float g, float b, float a) { 
    return ysMath::LoadVector(
        inverseGammaSrgb(r),
        inverseGammaSrgb(g),
        inverseGammaSrgb(b),
        inverseGammaSrgb(a)
    );
}

ysVector ysColor::srgbiToLinear(int r, int g, int b, int a) {
    return srgbToLinear(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

ysVector ysColor::linearToSrgb(const ysVector &srgb) {
    return ysMath::LoadVector(
        srgbGamma(ysMath::GetX(srgb)),
        srgbGamma(ysMath::GetY(srgb)),
        srgbGamma(ysMath::GetZ(srgb)),
        srgbGamma(ysMath::GetW(srgb))
    );
}

ysVector ysColor::srgbiToLinear(unsigned int rgb, float a) {
    int b = rgb & 0xFF; rgb >>= 8;
    int g = rgb & 0xFF; rgb >>= 8;
    int r = rgb & 0xFF; rgb >>= 8;

    return srgbToLinear(
        r / 255.0f, 
        g / 255.0f, 
        b / 255.0f, 
        a);
}

ysVector ysColor::srgbiToSrgb(unsigned int rgb, float a) {
    int b = rgb & 0xFF; rgb >>= 8;
    int g = rgb & 0xFF; rgb >>= 8;
    int r = rgb & 0xFF; rgb >>= 8;

    return ysMath::LoadVector(r / 255.0f, g / 255.0f, b / 255.0f, a);
}
