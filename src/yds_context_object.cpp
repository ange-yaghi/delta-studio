#include "../include/yds_context_object.h"

ysContextObject::ysContextObject() : ysObject("CONTEXT_OBJECT") {
	m_api = API_UNKNOWN;
	YDS_ERROR_RAISE(ysError::YDS_NO_PLATFORM);
}

ysContextObject::ysContextObject(const char *typeID, DEVICE_API API) : ysObject(typeID) {
	m_api = API;
}

ysContextObject::~ysContextObject() {
	/* void */
}
