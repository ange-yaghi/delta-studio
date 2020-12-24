#include "../include/yds_base.h"

#include <stdarg.h>
#include <stdio.h>

ysObject::ysObject() {
    m_typeID = "YS_OBJECT";
}

ysObject::ysObject(const char *typeID) {
    m_typeID = typeID;
}

ysObject::~ysObject() {
    /* void */
}

//#ifdef _DEBUG
//void ysObject::RaiseError(bool condition, const char *format, ...)
//{
//
//    if (!condition)
//    {
//
//        va_list argptr;
//        va_start(argptr, format);
//
//        char intermediateBuffer[1024];
//        vsprintf_s(intermediateBuffer, 1024, format, argptr);
//
//        char errorBuffer[2048];
//        sprintf_s(errorBuffer, 2048, "Type: %s\nName: %s\n--------------------\n%s", m_typeID, m_debugName, intermediateBuffer);
//
//        //yds_assert(condition, errorBuffer);
//
//    }
//
//}
//#endif
