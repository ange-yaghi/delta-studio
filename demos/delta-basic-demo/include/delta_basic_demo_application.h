#ifndef DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H
#define DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H

#include "delta_basic.h"

namespace dbasic_demo {

    class DeltaBasicDemoApplication {
    public:
        DeltaBasicDemoApplication();
        ~DeltaBasicDemoApplication();

        void Initialize(void *instance, ysContextObject::DEVICE_API api);
        void Run();

    protected:
        void Process();
        void Render();

        dbasic::DeltaEngine m_engine;
    };

} /* namesapce dbasic_demo */

#endif /* DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H */
