#include "../include/yds_timing.h"

#include <windows.h>
#include <mmsystem.h>

#include <intrin.h>

static bool qpcFlag;
static LARGE_INTEGER qpcFrequency;

ysTimingSystem *ysTimingSystem::g_instance = nullptr;

uint64_t SystemTime() {
    if (qpcFlag) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        currentTime.QuadPart *= 1000000;
        currentTime.QuadPart /= qpcFrequency.QuadPart;

        return (uint64_t)(currentTime.QuadPart);
    }
    else {
        // Convert output to microseconds
        return (uint64_t)(timeGetTime() * 1000);
    }
}

ysTimingSystem::ysTimingSystem() {
    SetPrecisionMode(Precision::Microsecond);
    Initialize();
}

ysTimingSystem::~ysTimingSystem() {
    /* void */
}

uint64_t ysTimingSystem::GetTime() {
    return SystemTime();
}

uint64_t SystemClock() {
    return __rdtsc();
}

uint64_t ysTimingSystem::GetClock() {
    return SystemClock();
}

void ysTimingSystem::SetPrecisionMode(Precision mode) {
    m_precisionMode = mode;

    if (mode == Precision::Millisecond) {
        m_div = 1000.0;
    }
    else if (mode == Precision::Microsecond) {
        m_div = 1000000.0;
    }
}

double ysTimingSystem::ConvertToSeconds(uint64_t t_u) {
    return t_u / m_div;
}

void ysTimingSystem::Update() {
    if (!m_isPaused) {
        m_frameNumber++;
    }

    uint64_t thisTime = GetTime();
    m_lastFrameDuration = (thisTime - m_lastFrameTimestamp);
    m_lastFrameTimestamp = thisTime;

    uint64_t thisClock = GetClock();
    m_lastFrameClockTicks = (thisClock - m_lastFrameClockstamp);
    m_lastFrameClockstamp = thisClock;

    if (m_frameNumber > 1) {
        if (m_averageFrameDuration <= 0) {
            m_averageFrameDuration = (double)(m_lastFrameDuration / m_div);
        }
        else {
            m_averageFrameDuration *= 0.95;
            m_averageFrameDuration += 0.05 * (double)m_lastFrameDuration / m_div;

            m_fps = (float)(1 / m_averageFrameDuration);
        }
    }
}

void ysTimingSystem::Initialize() {
#if _WIN32
    qpcFlag = (QueryPerformanceFrequency(&qpcFrequency) > 0);
#endif

    m_frameNumber = 0;

    m_lastFrameTimestamp = GetTime();
    m_lastFrameDuration = 0;

    m_lastFrameClockstamp = GetClock();
    m_lastFrameClockTicks = 0;

    m_isPaused = false;

    m_averageFrameDuration = 0;
    m_fps = 0;
}

double ysTimingSystem::GetFrameDuration() {
    return m_lastFrameDuration / m_div;
}

uint64_t ysTimingSystem::GetFrameDuration_us() {
    return m_lastFrameDuration;
}
