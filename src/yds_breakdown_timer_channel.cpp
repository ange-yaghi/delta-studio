#include "../include/yds_breakdown_timer_channel.h"

#include <assert.h>

ysBreakdownTimerChannel::ysBreakdownTimerChannel() {
    m_name = "";
    m_sampleBuffer = nullptr;
    m_currentBufferOffset = 0;
    m_bufferSize = 0;
    m_entryCount = 0;
}

ysBreakdownTimerChannel::~ysBreakdownTimerChannel() {
    assert(m_sampleBuffer == nullptr);
}

void ysBreakdownTimerChannel::Initialize(int bufferSize) {
    m_sampleBuffer = new float[bufferSize];
}

void ysBreakdownTimerChannel::Reset() {
    m_currentBufferOffset = 0;
    m_entryCount = 0;
}

void ysBreakdownTimerChannel::Destroy() {
    delete[] m_sampleBuffer;
    m_sampleBuffer = nullptr;
}

void ysBreakdownTimerChannel::RecordSample(float s) {
    int index = (m_currentBufferOffset + m_entryCount) % m_bufferSize;

    m_sampleBuffer[index] = s;
    m_entryCount++;
    if (m_entryCount >= m_bufferSize) m_entryCount = m_bufferSize;
}

float ysBreakdownTimerChannel::GetSample(int i) const {
    int index = (m_currentBufferOffset + i) % m_bufferSize;

    return m_sampleBuffer[(m_currentBufferOffset + index) % m_bufferSize];
}
