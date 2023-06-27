#ifndef DELTA_BASIC_EVENT_HANDLER_H
#define DELTA_BASIC_EVENT_HANDLER_H

namespace dbasic {
class EventHandler {
public:
    virtual void OnPause() {}
    virtual void OnUnpause() {}
};
}// namespace dbasic

#endif /* DELTA_BASIC_EVENT_HANDLER_H */
