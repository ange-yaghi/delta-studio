#include "../include/yds_color.h"

#include <cmath>

float ysColor::srgbGamma(float u) {
    constexpr float MinSrgbPower = 0.0031308f;

    if (u < MinSrgbPower) {
        return 12.92f * u;
    } else {
        return 1.055f * ::pow(u, 1 / 2.4f) - 0.055f;
    }
}

float ysColor::inverseGammaSrgb(float u) {
    static constexpr float MinSrgbPower = 0.04045f;

    if (u < MinSrgbPower) {
        return u / 12.92f;
    } else {
        return ::pow((u + 0.055f) / 1.055f, 2.4f);
    }
}

ysVector ysColor::srgbToLinear(float r, float g, float b, float a) {
    return ysMath::LoadVector(inverseGammaSrgb(r), inverseGammaSrgb(g),
                              inverseGammaSrgb(b), a);
}

ysVector ysColor::srgbiToLinear(int r, int g, int b, int a) {
    return srgbToLinear(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

ysVector ysColor::linearToSrgb(const ysVector &color) {
    return ysMath::LoadVector(
            srgbGamma(ysMath::GetX(color)), srgbGamma(ysMath::GetY(color)),
            srgbGamma(ysMath::GetZ(color)), ysMath::GetW(color));
}

ysVector ysColor::srgbaiToLinear(unsigned int rgba) {
    const int a = rgba & 0xFF;
    const int b = (rgba >>= 8) & 0xFF;
    const int g = (rgba >>= 8) & 0xFF;
    const int r = (rgba >>= 8) & 0xFF;
    return srgbToLinear(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

ysVector ysColor::srgbiToLinear(unsigned int rgb, float a) {
    const int b = rgb & 0xFF;
    const int g = (rgb >>= 8) & 0xFF;
    const int r = (rgb >>= 8) & 0xFF;
    return srgbToLinear(r / 255.0f, g / 255.0f, b / 255.0f, a);
}

ysVector ysColor::srgbiToSrgb(unsigned int rgb, float a) {
    const int b = rgb & 0xFF;
    const int g = (rgb >>= 8) & 0xFF;
    const int r = (rgb >>= 8) & 0xFF;
    return ysMath::LoadVector(r / 255.0f, g / 255.0f, b / 255.0f, a);
}

unsigned int ysColor::linearToSrgbi(const ysVector &color) {
    const ysVector srgb = linearToSrgb(color);
    const int r = int(std::roundf(ysMath::GetX(srgb) * 255.0f));
    const int g = int(std::roundf(ysMath::GetY(srgb) * 255.0f));
    const int b = int(std::roundf(ysMath::GetZ(srgb) * 255.0f));
    const int a = int(std::roundf(ysMath::GetW(srgb) * 255.0f));
    return a | (b << 8) | (g << 16) | (r << 24);
}
