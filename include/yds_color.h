#ifndef YDS_COLOR_H
#define YDS_COLOR_H

#include "yds_math.h"

namespace ysColor {

    float srgbGamma(float u);
    float inverseGammaSrgb(float u);

    ysVector srgbToLinear(float r, float g, float b, float a = 1.0f);
    ysVector srgbiToLinear(int r, int g, int b, int a = 255);
    ysVector linearToSrgb(const ysVector &color);

    ysVector srgbaiToLinear(unsigned int rgba);
    ysVector srgbiToLinear(unsigned int rgb, float a = 1.0f);
    ysVector srgbiToSrgb(unsigned int rgb, float a = 1.0f);
    unsigned int linearToSrgbi(const ysVector &color);

} /* namespace ysColor */

#endif /* YDS_COLOR_H */
