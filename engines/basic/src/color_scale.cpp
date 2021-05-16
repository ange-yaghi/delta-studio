#include "../include/color_scale.h"

ysVector4 dbasic::ColorScale3(float x, const ysVector &col1, const ysVector &col2, const ysVector &col3) {
    if (x <= 0.0f) return ysMath::GetVector4(col1);
    else if (x >= 1.0f) return ysMath::GetVector4(col3);

    if (x <= 0.5f) {
        ysVector lerp;

        float s = x / 0.5f;

        ysVector sv = ysMath::LoadScalar(s);
        ysVector inv = ysMath::Sub(ysMath::Constants::One, sv);

        lerp = ysMath::Add(ysMath::Mul(sv, col2), ysMath::Mul(inv, col1));

        return ysMath::GetVector4(lerp);
    }
    else if (x > 0.5f) {
        ysVector lerp;

        float s = (x - 0.5f) / 0.5f;

        ysVector sv = ysMath::LoadScalar(s);
        ysVector inv = ysMath::Sub(ysMath::Constants::One, sv);

        lerp = ysMath::Add(ysMath::Mul(sv, col3), ysMath::Mul(inv, col2));

        return ysMath::GetVector4(lerp);
    }

    // Shouldn't get here
    return ysMath::GetVector4(ysMath::Constants::Zero);
}
