#ifndef YS_TIMING_MODULE_H
#define YS_TIMING_MODULE_H

#include <intrin.h>
#include <stdint.h>

uint64_t SystemTime();

class ysTimingSystem
{

public:

	enum PRECISION_MODE
	{

		MICROSECOND_MODE,
		MILLISECOND_MODE

	};

protected:

	static ysTimingSystem *g_instance;

public:

	ysTimingSystem();
	~ysTimingSystem();

	static ysTimingSystem *Get() 
	{ 

		if (g_instance == NULL) 
			g_instance = new ysTimingSystem; 

		return g_instance;
	
	}

	void Update();
    void Initialize();

	double GetFrameDuration();
	uint64_t GetFrameDuration_us();

    uint64_t GetTime();
    unsigned __int64 GetClock();

	void SetPrecisionMode(PRECISION_MODE mode);

protected:

	PRECISION_MODE m_precisionMode;
	double m_div;

    unsigned m_frameNumber;

	uint64_t m_lastFrameTimestamp;
	uint64_t m_lastFrameDuration;

    unsigned long long m_lastFrameClockstamp;
    unsigned long long m_lastFrameClockTicks;

    bool m_isPaused;

    double m_averageFrameDuration;
    float m_fps;

};

#endif