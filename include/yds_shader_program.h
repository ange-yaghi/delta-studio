#ifndef YDS_SHADER_PROGRAM_H
#define YDS_SHADER_PROGRAM_H

#include "yds_context_object.h"
#include "yds_shader.h"

class ysShaderProgram : public ysContextObject
{

	friend class ysDevice;

public:

	ysShaderProgram();
	ysShaderProgram(DEVICE_API API);
	virtual ~ysShaderProgram();

	const ysShader *GetShader(ysShader::SHADER_TYPE type) const { return m_shaderSlots[type]; }

protected:

	ysShader *m_shaderSlots[ysShader::SHADER_TYPE_NUM_TYPES];
	bool m_isLinked;

};

#endif