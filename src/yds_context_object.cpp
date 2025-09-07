#include "yds_context_object.h"

#include "yds_error_system.h"

ysContextObject::ysContextObject()
    : ysObject("CONTEXT_OBJECT")
    , m_api(DeviceAPI::Unknown)

{
    YDS_ERROR_RAISE(ysError::NoPlatform);
}

ysContextObject::ysContextObject(const char *typeID, DeviceAPI api)
    : ysObject(typeID)
    , m_api(api)
{ }

ysContextObject::~ysContextObject() {
    /* void */
}

ysContextObject::DeviceAPI ysContextObject::GetAPI() const
{
    return m_api;
}

bool ysContextObject::CheckCompatibility(const ysContextObject *object) const {
    return (object != nullptr)
        ? object->m_api == m_api
        : true;
}
