#include "../include/yds_breakdown_timer_channel.h"

#include "../include/yds_timing.h"

#include <assert.h>

ysBreakdownTimerChannel::ysBreakdownTimerChannel() {
    m_name = "";
    m_sampleBuffer = nullptr;
    m_currentWriteIndex = 0;
    m_bufferSize = 0;
    m_entryCount = 0;
    m_midMeasurement = false;
    m_frameCount = 0;
}

ysBreakdownTimerChannel::~ysBreakdownTimerChannel() {
    if (m_sampleBuffer != nullptr) Destroy();
}

void ysBreakdownTimerChannel::Initialize(int bufferSize) {
    m_bufferSize = bufferSize;
    m_sampleBuffer = new double[m_bufferSize];
}

void ysBreakdownTimerChannel::Reset() {
    m_currentWriteIndex = 0;
    m_entryCount = 0;
    m_frameCount = 0;
}

void ysBreakdownTimerChannel::Destroy() {
    delete[] m_sampleBuffer;
    m_sampleBuffer = nullptr;
}

void ysBreakdownTimerChannel::RecordSample(double s) {
    int index = m_currentWriteIndex;

    m_sampleBuffer[index] = s;
    m_entryCount = (m_entryCount >= m_bufferSize)
        ? m_bufferSize
        : m_entryCount + 1;
    m_currentWriteIndex = (m_currentWriteIndex + 1) % m_bufferSize;
    ++m_frameCount;
}

double ysBreakdownTimerChannel::GetSample(int i) const {
    int index = (m_currentWriteIndex - m_entryCount + i + m_bufferSize) % m_bufferSize;

    return m_sampleBuffer[index];
}

double ysBreakdownTimerChannel::GetLastSample() const {
    if (m_entryCount == 0) return 0.0f;
    else return GetSample(m_entryCount - 1);
}

void ysBreakdownTimerChannel::StartMeasurement(uint64_t timestamp) {
    assert(!m_midMeasurement);

    m_lastMeasurementStart = timestamp;
    m_midMeasurement = true;
}

void ysBreakdownTimerChannel::EndMeasurement(uint64_t timestamp) {
    assert(m_midMeasurement);

    double s = ysTimingSystem::Get()->ConvertToSeconds(timestamp - m_lastMeasurementStart);
    RecordSample(s);

    m_midMeasurement = false;
}
