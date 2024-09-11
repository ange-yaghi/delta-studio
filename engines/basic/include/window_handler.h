#ifndef DELTA_BASIC_DELTA_WINDOW_HANDLER_H
#define DELTA_BASIC_DELTA_WINDOW_HANDLER_H

#include "delta_core.h"

#if defined(_WIN64)

namespace dbasic {

class DeltaEngine;

class WindowHandler : public ysWindowEventHandler {
public:
    WindowHandler();
    virtual ~WindowHandler();

    void Initialize(ysDevice *device, ysRenderingContext *context,
                    DeltaEngine *engine);

    virtual void OnStartMoveResize() override;
    virtual void OnEndMoveResize() override;
    virtual void OnResizeWindow(int width, int height) override;

protected:
    ysDevice *m_device;
    ysRenderingContext *m_context;
    DeltaEngine *m_engine;
};

}// namespace dbasic

#endif /* Windows */

#endif /* DELTA_BASIC_DELTA_WINDOW_HANDLER_H */
