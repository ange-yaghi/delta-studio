#ifndef DELTA_MULTIPLE_WINDOW_DEMO_H
#define DELTA_MULTIPLE_WINDOW_DEMO_H

#include "demo_application.h"

#include "delta.h"

namespace delta_demo {

    class MultipleWindowDemo : public DemoApplication {
    public:
        MultipleWindowDemo();
        virtual ~MultipleWindowDemo();

        virtual void Initialize(void *instance, ysContextObject::DeviceAPI api);
        virtual void Run();
        virtual void Destroy();

    protected:
        ysWindowSystem *m_windowSystem;

        ysRenderingContext *m_context1;
        ysRenderingContext *m_context2;

        ysRenderTarget *m_renderTarget1;
        ysRenderTarget *m_renderTarget2;

        ysWindow *m_window1;
        ysWindow *m_window2;

        ysDevice *m_device;

    protected:
        void Process();
        void Render();
    };

} /* namesapce dbasic_demo */

#endif /* DELTA_MULTIPLE_WINDOW_DEMO_H */
