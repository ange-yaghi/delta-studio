#include "yds_window_system_object.h"

#include "yds_error_system.h"

ysWindowSystemObject::ysWindowSystemObject()
    : ysObject("WINDOW_SYSTEM_OBJECT")
    , m_platform(Platform::Unknown)
{ }

ysWindowSystemObject::ysWindowSystemObject(const char *typeID, Platform platform)
    : ysObject(typeID)
    , m_platform(platform)
{ }

ysWindowSystemObject::Platform ysWindowSystemObject::GetPlatform() const
{
    return m_platform; 
}

bool ysWindowSystemObject::CheckCompatibility(ysWindowSystemObject *object) const
{
    return (object) ? object->m_platform == m_platform : true; 
}