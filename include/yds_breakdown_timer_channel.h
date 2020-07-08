#ifndef YDS_BREAKDOWN_TIMER_CHANNEL_H
#define YDS_BREAKDOWN_TIMER_CHANNEL_H

#include "yds_base.h"

#include <string>

class ysBreakdownTimerChannel : public ysObject {
public:
    ysBreakdownTimerChannel();
    ~ysBreakdownTimerChannel();

    void Initialize(int bufferSize);
    void Reset();
    void Destroy();

    void SetName(const std::string &name) { m_name = name; }
    std::string GetName() const { return m_name; }

    int GetEntryCount() const { return m_entryCount; }

    void RecordSample(float s);
    float GetSample(int i) const;

protected:
    std::string m_name;
    float *m_sampleBuffer;
    int m_bufferSize;
    int m_currentBufferOffset;
    int m_entryCount;
};

#endif /* YDS_BREAKDOWN_TIMER_CHANNEL_H */
