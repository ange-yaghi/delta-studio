#ifndef YDS_BASE_H
#define YDS_BASE_H

#include "yds_dynamic_array.h"
#include "yds_syntax.h"

class ysObject : public ysDynamicArrayElement
{

public:

	ysObject();
	ysObject(const char *typeID);
	~ysObject();

//#ifdef _DEBUG
//	void RaiseError(bool condition, const char *format, ...);
//#else
//#define RaiseError(condition, format, ...) ((void)0)
//#endif

	void SetDebugName(const char *debugName);
	const char *GetDebugName() const { return m_debugName; }
	const char *GetTypeID() const { return m_typeID; }

protected:

	const char *m_debugName;
	const char *m_typeID;

};


// TEMP
#include "yds_error_system.h"

#endif