#ifndef YDS_BREAKDOWN_TIMER_H
#define YDS_BREAKDOWN_TIMER_H

#include "yds_base.h"

#include "yds_expanding_array.h"
#include "yds_breakdown_timer_channel.h"

class ysBreakdownTimer : public ysObject {
public:
    ysBreakdownTimer();
    ~ysBreakdownTimer();

    int GetChannelCount() const { return m_channels.GetNumObjects(); }
    
    void StartFrame();
    void StartMeasurement(const std::string &timerChannelName);
    void EndMasurement(const std::string &timerChannelName);

protected:
    ysBreakdownTimerChannel *FindChannel(const std::string &s);

    ysExpandingArray<ysBreakdownTimerChannel, 4> m_channels;
    ysExpandingArray<std::string, 4> m_executionOrder;
};

#endif /* YDS_BREAKDOWN_TIMER_H */
