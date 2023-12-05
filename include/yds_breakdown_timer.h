#ifndef YDS_BREAKDOWN_TIMER_H
#define YDS_BREAKDOWN_TIMER_H

#include "yds_base.h"

#include "yds_expanding_array.h"
#include "yds_dynamic_array.h"

#include <fstream>
#include <string>
#include <vector>

class ysBreakdownTimerChannel;

class ysBreakdownTimer : public ysObject {
public:
    ysBreakdownTimer();
    ~ysBreakdownTimer();

    int GetChannelCount() const { return m_channels.GetNumObjects(); }
    ysBreakdownTimerChannel *GetChannel(int i) const { return m_channels.Get(i); }

    void ResetChannels();
    void Clear();
    
    void StartFrame();
    void EndFrame();

    void StartMeasurement(const std::string &timerChannelName);
    void EndMeasurement(const std::string &timerChannelName);
    void SkipMeasurement(const std::string &timerChannelName);

    ysBreakdownTimerChannel *CreateChannel(const std::string &timerChannelName, int bufferSize = 1024);

    uint64_t GetFrameCount() const { return m_frameCount; }

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    void OpenLogFile(const std::wstring &filename);
    void WriteLastFrameToLogFile();
    void CloseLogFile();

protected:
    ysBreakdownTimerChannel *FindChannel(const std::string &timerChannelName);

    ysDynamicArray<ysBreakdownTimerChannel, 4> m_channels;
    std::vector<std::string> m_executionOrder;

    std::fstream m_logFile;

    uint64_t m_frameCount;
    bool m_enabled;
};

#endif /* YDS_BREAKDOWN_TIMER_H */
