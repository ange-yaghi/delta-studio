#ifndef DELTA_DEMO_DEMO_LIST_H
#define DELTA_DEMO_DEMO_LIST_H

#include "demo_factory.h"
#include "demo_application.h"
#include "all_demos.h"

DEMO_LIST()
    DEMO(DemoApplication::Demo::DeltaBasicDemo, DeltaBasicDemoApplication)
    DEMO(DemoApplication::Demo::EmptyWindowDemo, EmptyWindowDemo)
    DEMO(DemoApplication::Demo::MultipleWindowDemo, MultipleWindowDemo)
END_DEMO_LIST()

#endif /* DELTA_DEMO_DEMO_LIST_H */
