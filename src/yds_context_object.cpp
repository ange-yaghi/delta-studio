#include "../include/yds_context_object.h"

ysContextObject::ysContextObject() : ysObject("CONTEXT_OBJECT") {
    m_api = DeviceAPI::Unknown;
    YDS_ERROR_RAISE(ysError::NoPlatform);
}

ysContextObject::ysContextObject(const char *typeID, DeviceAPI API) : ysObject(typeID) {
    m_api = API;
}

ysContextObject::~ysContextObject() {
    /* void */
}
