#ifndef APTSIM_APARTMENT_SIMULATOR_WINDOW_HANDLER_H
#define APTSIM_APARTMENT_SIMULATOR_WINDOW_HANDLER_H

#include "../../../include/yds_device.h"
#include "../../../include/yds_window_event_handler.h"

class ApartmentSimulatorWindowHandler : public ysWindowEventHandler {
public:
    ApartmentSimulatorWindowHandler() {
        SetDebugName("ApartmentSimulatorWindow");

        m_context = NULL;
        m_device = NULL;
    }

    virtual ~ApartmentSimulatorWindowHandler() {}

    virtual void OnResizeWindow(int w, int h) {
        char newTitle[64];
        sprintf_s(newTitle, 64, "SIZE %d %d", w, h);
        GetWindow()->SetTitle(newTitle);

        if (m_device != NULL && m_context != NULL) {
            m_device->UpdateRenderingContext(m_context);
        }
    }

    ysDevice *m_device;
    ysRenderingContext *m_context;
};

#endif /* APTSIM_APARTMENT_SIMULATOR_WINDOW_HANDLER_H */
