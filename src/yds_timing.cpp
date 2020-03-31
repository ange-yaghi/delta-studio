#include "../include/yds_timing.h"

#include <windows.h>
#include <mmsystem.h>

static bool qpcFlag;
static LARGE_INTEGER qpcFrequency;

ysTimingSystem *ysTimingSystem::g_instance = NULL;

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
    SetPrecisionMode(MICROSECOND_MODE);
    Initialize();
}

ysTimingSystem::~ysTimingSystem() {
    /* void */
}

uint64_t ysTimingSystem::GetTime() {
    return SystemTime();
}

inline unsigned __int64 SystemClock() {
    return __rdtsc();
}

unsigned __int64 ysTimingSystem::GetClock() {
    return (unsigned long long) SystemClock();
}

void ysTimingSystem::SetPrecisionMode(PRECISION_MODE mode) {
    m_precisionMode = mode;

    if (mode == MILLISECOND_MODE) {
        m_div = 1000.0;
    }
    else if (mode == MICROSECOND_MODE) {
        m_div = 1000000.0;
    }
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
    qpcFlag = (QueryPerformanceFrequency(&qpcFrequency) > 0);

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
