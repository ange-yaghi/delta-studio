#ifndef DELTA_DEMO_DEMO_FACTORY_H
#define DELTA_DEMO_DEMO_FACTORY_H

#define DEMO(label, type) \
    case (label): return new type;

#define DEMO_LIST() \
namespace delta_demo { \
    DemoApplication *CreateApplication(DemoApplication::Demo demo) { \
        switch(demo) {

#define END_DEMO_LIST() \
        default: return nullptr; \
        } \
    } \
} /* namespace delta_demo */

#endif /* DELTA_DEMO_DEMO_FACTORY_H */
