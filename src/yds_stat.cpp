#include "../include/yds_stat.h"

#include <math.h>

float ysStat::RandomNumber() {
    return rand() / ((float)RAND_MAX);
}

bool ysStat::Decide(float probability) {
    return RandomNumber() <= probability;
}

bool ysStat::Decide(float frequency, float timePassed) {
    return ysStat::Decide(frequency * timePassed);
}

// Stolen from Wikipedia
float ysStat::NormalRandomNumber(float variance) {
    static bool spareAvailable = false;
    static float rand1, rand2;

    if (spareAvailable) {
        spareAvailable = false;
        return ::sqrtf(variance * rand1) * ::sinf(rand2);
    }

    spareAvailable = true;

    rand1 = ysStat::RandomNumber();
    if (rand1 < 1e-14) rand1 = 1e-14f;

    rand1 = -2 * logf(rand1);
    rand2 = ysStat::RandomNumber() * ysMath::Constants::TWO_PI;

    return ::sqrtf(variance * rand1) * ::cosf(rand2);
}

float ysStat::NormalRandomNumber(float mean, float variance) {
    return ysStat::NormalRandomNumber(variance) + mean;
}
