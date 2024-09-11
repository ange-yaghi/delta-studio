#include "../include/yds_breakdown_timer.h"

#include "../include/yds_breakdown_timer_channel.h"
#include "../include/yds_timing.h"

#include <assert.h>

ysBreakdownTimer::ysBreakdownTimer() {
    m_frameCount = 0;
    m_enabled = true;
}

ysBreakdownTimer::~ysBreakdownTimer() {}

void ysBreakdownTimer::ResetChannels() {
    const int n = GetChannelCount();
    for (int i = 0; i < n; ++i) {
        ysBreakdownTimerChannel *channel = m_channels.Get(i);
        channel->Reset();
    }

    m_frameCount = 0;
}

void ysBreakdownTimer::Clear() {
    m_frameCount = 0;

    m_executionOrder.clear();
    m_channels.Clear();
}

void ysBreakdownTimer::StartFrame() { m_executionOrder.clear(); }
void ysBreakdownTimer::EndFrame() { ++m_frameCount; }

#if defined(_WIN64) // YSTimingSystem 

void ysBreakdownTimer::StartMeasurement(const std::string &timerChannelName) {
    ysBreakdownTimerChannel *channel = FindChannel(timerChannelName);
    assert(channel != nullptr);

    m_executionOrder.push_back(timerChannelName);

    uint64_t timestamp = ysTimingSystem::Get()->GetTime();

    channel->StartMeasurement(timestamp);

    uint64_t frameCount = channel->GetFrameCount();
    assert(frameCount == m_frameCount);
}

void ysBreakdownTimer::EndMeasurement(const std::string &timerChannelName) {
    ysBreakdownTimerChannel *channel = FindChannel(timerChannelName);
    assert(channel != nullptr);

    uint64_t timestamp = ysTimingSystem::Get()->GetTime();

    channel->EndMeasurement(timestamp);
}

#endif /* Windows */

void ysBreakdownTimer::SkipMeasurement(const std::string &timerChannelName) {
    StartMeasurement(timerChannelName);
    EndMeasurement(timerChannelName);
}

ysBreakdownTimerChannel *
ysBreakdownTimer::CreateChannel(const std::string &timerChannelName,
                                int bufferSize) {
    ysBreakdownTimerChannel *newChannel = m_channels.New();
    newChannel->SetName(timerChannelName);
    newChannel->Initialize(bufferSize);

    return newChannel;
}

void ysBreakdownTimer::OpenLogFile(const std::wstring &filename) {
    if (!m_enabled) { return; }

    m_logFile.open(filename.c_str(), std::ios::out);

    m_logFile << "Frame";
    const int n = GetChannelCount();
    for (int i = 0; i < n; ++i) {
        ysBreakdownTimerChannel *channel = m_channels.Get(i);
        m_logFile << ", " << channel->GetName();
    }

    m_logFile << "\n";
}

void ysBreakdownTimer::WriteLastFrameToLogFile() {
    if (!m_enabled) { return; }
    if (m_frameCount == 0) { return; }
    if (!m_logFile.is_open()) { return; }

    m_logFile << m_frameCount;

    const int n = GetChannelCount();
    for (int i = 0; i < n; ++i) {
        ysBreakdownTimerChannel *channel = m_channels.Get(i);
        assert(!channel->IsMidMeasurement());
        assert(channel->GetFrameCount() == m_frameCount);

        m_logFile << ", " << channel->GetLastSample();
    }

    m_logFile << "\n";
}

void ysBreakdownTimer::CloseLogFile() {
    if (m_logFile.is_open()) { m_logFile.close(); }
}

ysBreakdownTimerChannel *ysBreakdownTimer::FindChannel(const std::string &s) {
    const int n = GetChannelCount();
    for (int i = 0; i < n; ++i) {
        if (m_channels.Get(i)->GetName() == s) { return m_channels.Get(i); }
    }

    return nullptr;
}
