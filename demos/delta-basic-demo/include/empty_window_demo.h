#ifndef DELTA_EMPTY_WINDOW_DEMO_H
#define DELTA_EMPTY_WINDOW_DEMO_H

#include "demo_application.h"

#include "delta.h"

namespace delta_demo {

    class EmptyWindowDemo : public DemoApplication {
    public:
        EmptyWindowDemo();
        virtual ~EmptyWindowDemo();

        virtual void Initialize(void *instance, ysContextObject::DeviceAPI api);
        virtual void Run();
        virtual void Destroy();

    protected:
        ysWindowSystem *m_windowSystem;
        ysRenderingContext *m_context;
        ysRenderTarget *m_renderTarget;
        ysWindow *m_window;
        ysDevice *m_device;

    protected:
        void Process();
        void Render();
    };

} /* namesapce dbasic_demo */

#endif /* DELTA_EMPTY_WINDOW_DEMO_H */
