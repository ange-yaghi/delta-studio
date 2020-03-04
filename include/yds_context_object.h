#ifndef YDS_CONTEXT_OBJECT_H
#define YDS_CONTEXT_OBJECT_H

#include "yds_base.h"

class ysContextObject : public ysObject
{

public:

	enum DEVICE_API
	{

		API_UNKNOWN,
		DIRECTX10,
		DIRECTX11,
		OPENGL4_0

	};

public:

	ysContextObject();
	ysContextObject(const char *typeID, DEVICE_API API);
	virtual ~ysContextObject();

	DEVICE_API GetAPI() const { return m_api; }

	bool CheckCompatibility(ysContextObject *object) const { return (object) ? object->m_api == m_api : true; }

private:

	DEVICE_API m_api;

};

#endif