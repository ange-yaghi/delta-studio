#include "../include/yds_timing.h"

#include "../include/yds_math.h"

#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>

#include <intrin.h>

static bool qpcFlag;
static LARGE_INTEGER qpcFrequency;

ysTimingSystem *ysTimingSystem::g_instance = nullptr;

uint64_t SystemTime() {
    if (qpcFlag) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        return uint64_t(currentTime.QuadPart);
    } else {
        // Convert output to microseconds
        return uint64_t(timeGetTime()) * 1000;
    }
}

ysTimingSystem::ysTimingSystem() {
    SetPrecisionMode(Precision::Microsecond);
    Initialize();
    m_averageTimer = 0.0;
}

ysTimingSystem::~ysTimingSystem() { /* void */
}

uint64_t ysTimingSystem::GetTime() { return SystemTime(); }

inline unsigned __int64 SystemClock() { return __rdtsc(); }

unsigned __int64 ysTimingSystem::GetClock() {
    return (unsigned long long) SystemClock();
}

void ysTimingSystem::SetPrecisionMode(Precision mode) {
    m_precisionMode = mode;

    if (mode == Precision::Millisecond) {
        m_div = 1000.0;
    } else if (mode == Precision::Microsecond) {
        m_div = 1000000.0;
    }
}

double ysTimingSystem::ConvertToSeconds(uint64_t t_u) { return t_u / m_div; }

void ysTimingSystem::Update() {
    if (!m_isPaused) { m_frameNumber++; }

    const uint64_t thisTime = GetTime();
    m_lastFrameDuration = ((thisTime - m_lastFrameTimestamp) * 1000000) /
                          qpcFrequency.QuadPart;
    m_lastFrameTimestamp = thisTime;

    const uint64_t thisClock = GetClock();
    m_lastFrameClockTicks = (thisClock - m_lastFrameClockstamp);
    m_lastFrameClockstamp = thisClock;

    const double frameDuration = GetFrameDuration();
    if (m_frameNumber > 1) {
        constexpr double dt = 1 / 10000.0;
        const double f_c = (m_frameNumber > 1000) ? 0.1 : 100.0;
        const double RC = 1.0 / (ysMath::Constants::TWO_PI * f_c);
        const double alpha = dt / (RC + dt);
        m_averageTimer += frameDuration;
        if (m_averageTimer > 1.0) { m_averageTimer = 1.0; }
        while (m_averageTimer > 0) {
            m_averageTimer -= dt;
            m_averageFrameDuration = alpha * frameDuration +
                                     (1 - alpha) * m_averageFrameDuration;
        }
    } else {
        m_averageFrameDuration = frameDuration;
    }

    m_fps = float(1 / m_averageFrameDuration);
}

void ysTimingSystem::RestartFrame() {
    const uint64_t thisTime = GetTime();
    m_lastFrameTimestamp = thisTime;
}

void ysTimingSystem::Initialize() {
    qpcFlag = (QueryPerformanceFrequency(&qpcFrequency) > 0);

    m_frameNumber = 0;

    m_lastFrameTimestamp = GetTime();
    m_lastFrameDuration = 0;

    m_lastFrameClockstamp = GetClock();
    m_lastFrameClockTicks = 0;

    m_isPaused = false;

    m_fps = 1024.0;
}

double ysTimingSystem::GetFrameDuration() {
    return m_lastFrameDuration / m_div;
}

uint64_t ysTimingSystem::GetFrameDuration_us() { return m_lastFrameDuration; }
