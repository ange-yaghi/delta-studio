#ifndef DELTA_BASIC_DELTA_WINDOW_HANDLER_H
#define DELTA_BASIC_DELTA_WINDOW_HANDLER_H

#include "delta_core.h"

namespace dbasic {

class DeltaEngine;

class WindowHandlerAttachment {
public:
    virtual void OnClose() = 0;
};

class WindowHandler : public ysWindowEventHandler {
public:
    WindowHandler();
    virtual ~WindowHandler();

    void Initialize(ysDevice *device, ysRenderingContext *context,
                    DeltaEngine *engine);

    inline void SetAttachment(WindowHandlerAttachment *attachment) {
        m_attachment = attachment;
    }

    virtual void OnStartMoveResize() override;
    virtual void OnEndMoveResize() override;
    virtual void OnResizeWindow(int width, int height) override;
    virtual void OnCloseWindow() override;

protected:
    WindowHandlerAttachment *m_attachment;
    ysDevice *m_device;
    ysRenderingContext *m_context;
    DeltaEngine *m_engine;
};

}// namespace dbasic

#endif /* DELTA_BASIC_DELTA_WINDOW_HANDLER_H */
