#include "../include/yds_breakdown_timer.h"

ysBreakdownTimer::ysBreakdownTimer() {
    /* void */
}

ysBreakdownTimer::~ysBreakdownTimer() {
    /* void */
}

void ysBreakdownTimer::StartFrame() {
    m_executionOrder.Clear();
}

void ysBreakdownTimer::StartMeasurement(const std::string &timerChannelName) {

}

void ysBreakdownTimer::EndMasurement(const std::string &timerChannelName) {

}

ysBreakdownTimerChannel *ysBreakdownTimer::FindChannel(const std::string &s) {
    return nullptr;
}
