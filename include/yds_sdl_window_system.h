#pragma once

#include "yds_window_system.h"

class ysSdlWindowSystem : public ysWindowSystem {
public:
    ysSdlWindowSystem();
    ~ysSdlWindowSystem();

    virtual ysError NewWindow(ysWindow **window) override;
    virtual ysMonitor *NewMonitor() override;
    virtual void SurveyMonitors() override;
    virtual void ProcessMessages() override;
};

