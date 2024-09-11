#include "../include/yds_window_event_handler.h"

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)

// TODO: -


#elif defined(_WIN64)

ysWindowEventHandler::ysWindowEventHandler() : ysObject("WINDOW_EVENT_HANDLER") {
    m_window = nullptr;
}

ysWindowEventHandler::~ysWindowEventHandler() {
    /* void */
}

#endif /* Windows */
