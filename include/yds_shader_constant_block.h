#ifndef YDS_SHADER_CONSTANT_BLOCK_H
#define YDS_SHADER_CONSTANT_BLOCK_H

#include "yds_context_object.h"

class ysShaderConstantBlock : public ysContextObject
{

public:

	ysShaderConstantBlock();
	ysShaderConstantBlock(DEVICE_API API);
	virtual ~ysShaderConstantBlock();

};

#endif