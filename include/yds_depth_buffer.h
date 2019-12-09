#ifndef YDS_DEPTH_BUFFER_H
#define YDS_DEPTH_BUFFER_H

#include "yds_context_object.h"

class ysDepthBuffer : public ysContextObject
{

public:

	ysDepthBuffer();
	ysDepthBuffer(DEVICE_API API);
	virtual ~ysDepthBuffer();

};

#endif