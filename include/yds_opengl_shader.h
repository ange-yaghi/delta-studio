#include "yds_shader.h"

class ysOpenGLShader : public ysShader
{

	friend class ysOpenGLDevice;

public:

	ysOpenGLShader();
	~ysOpenGLShader();

protected:

	unsigned int m_handle;

};