#ifndef YDS_SHADER_H
#define YDS_SHADER_H

#include "yds_context_object.h"

class ysShader : public ysContextObject
{

	friend class ysDevice;

public:

	static const int MAX_SHADER_FILENAME_LENGTH = 256;
	static const int MAX_SHADER_NAME = 64;

	enum SHADER_TYPE
	{
	
		SHADER_TYPE_PIXEL,
		SHADER_TYPE_VERTEX,

		SHADER_TYPE_NUM_TYPES

	};

public:

	ysShader();
	ysShader(DEVICE_API API);
	virtual ~ysShader();

	const char *GetFilename() const { return m_filename; }
	const char *GetShaderName() const { return m_shaderName; }

	SHADER_TYPE GetShaderType() const { return m_shaderType; }

protected:

	char m_filename[MAX_SHADER_FILENAME_LENGTH];
	char m_shaderName[MAX_SHADER_NAME];

	SHADER_TYPE m_shaderType;

};

#endif