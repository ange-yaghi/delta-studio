#ifndef YDS_CONTEXT_OBJECT_H
#define YDS_CONTEXT_OBJECT_H

#include "yds_base.h"

class ysContextObject : public ysObject {
public:
	enum class DeviceAPI {
		Unknown,
		DirectX10,
		DirectX11,
		OpenGL4_0
	};

public:
	ysContextObject();
	ysContextObject(const char *typeID, DeviceAPI API);
	virtual ~ysContextObject();

	DeviceAPI GetAPI() const { return m_api; }

	bool CheckCompatibility(ysContextObject *object) const { return (object) ? object->m_api == m_api : true; }

private:
	DeviceAPI m_api;
};

#endif /* YDS_CONTEXT_OBJECT_H */
