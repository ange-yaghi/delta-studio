#include "../include/yds_window_system_object.h"

ysWindowSystemObject::ysWindowSystemObject() : ysObject("WINDOW_SYSTEM_OBJECT") {
    YDS_ERROR_DECLARE("ysWindowSystemObject");

    m_platform = Platform::Unknown;

    YDS_ERROR_RETURN(ysError::NoPlatform);
}

ysWindowSystemObject::ysWindowSystemObject(const char *typeID, Platform platform) : ysObject(typeID) {
    m_platform = platform;
}

ysWindowSystemObject::~ysWindowSystemObject() {
    /* void */
}
