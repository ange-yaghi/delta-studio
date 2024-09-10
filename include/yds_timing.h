#ifndef YS_TIMING_H
#define YS_TIMING_H

#include <stdint.h>

uint64_t SystemTime();

class ysTimingSystem {
public:
    enum class Precision { Microsecond, Millisecond };

protected:
    static ysTimingSystem *g_instance;

public:
    ysTimingSystem();
    ~ysTimingSystem();

    static ysTimingSystem *Get() {
        if (g_instance == nullptr) { g_instance = new ysTimingSystem; }

        return g_instance;
    }

    void Update();
    void RestartFrame();
    void Initialize();

    double GetFrameDuration();
    uint64_t GetFrameDuration_us();

    uint64_t GetTime();
    unsigned __int64 GetClock();

    void SetPrecisionMode(Precision mode);
    Precision GetPrecisionMode() const { return m_precisionMode; }

    double ConvertToSeconds(uint64_t t_u);

    inline float GetFPS() const { return m_fps; }
    inline double GetAverageFrameDuration() const {
        return m_averageFrameDuration;
    }

protected:
    Precision m_precisionMode;
    double m_div;

    unsigned long m_frameNumber;

    uint64_t m_lastFrameTimestamp;
    uint64_t m_lastFrameDuration;

    unsigned long long m_lastFrameClockstamp;
    unsigned long long m_lastFrameClockTicks;

    bool m_isPaused;

    double m_averageFrameDuration;
    double m_averageTimer;
    float m_fps;
};

#endif /* YS_TIMING_H */
