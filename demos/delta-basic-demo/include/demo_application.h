#ifndef DELTA_DEMO_DEMO_APPLICATION_H
#define DELTA_DEMO_DEMO_APPLICATION_H

#include "delta.h"

namespace delta_demo {

    class DemoApplication {
    public:
        enum class Demo {
            DeltaBasicDemo,
            EmptyWindowDemo,
            MultipleWindowDemo
        };

    public:
        DemoApplication();
        virtual ~DemoApplication();

        virtual void Initialize(void *instance, ysContextObject::DeviceAPI api);
        virtual void Destroy();
        virtual void Run();
    };

} /* namespace delta_demo */

#endif /* DELTA_DEMO_DEMO_APPLICATION_H */
