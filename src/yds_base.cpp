#include "yds_base.h"

ysObject::ysObject()
    : m_typeID("YS_OBJECT")
{ }

ysObject::ysObject(const char *typeID)
    : m_typeID(typeID)
{ }

const char* ysObject::GetTypeID() const
{
    return m_typeID;
}