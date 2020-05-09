#ifndef YDS_OPENGL_CONTEXT_H
#define YDS_OPENGL_CONTEXT_H

#include "yds_window.h"
#include "yds_rendering_context.h"

#include <OpenGL.h>

class ysOpenGLVirtualContext : public ysRenderingContext {
	friend class ysOpenGLDevice;

public:
	ysOpenGLVirtualContext();
	ysOpenGLVirtualContext(ysWindowSystemObject::Platform platform);
	virtual ~ysOpenGLVirtualContext();

	virtual ysError DestroyContext() {
		return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); 
	}

	virtual ysError TransferContext(ysOpenGLVirtualContext *context) {
		(void)context; 
		return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); 
	}

	virtual ysError SetContextMode(ContextMode mode) {
		(void)mode; 
		return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); 
	}

	virtual ysError SetContext(ysRenderingContext *realContext) {
		(void)realContext; 
		return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); 
	}

	virtual ysError Present() {
		return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR); 
	}

	// OpenGL Extensions
	PFNGLGENBUFFERSPROC					glGenBuffers;
	PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
	PFNGLDELETEVERTEXARRAYSPROC			glDeleteVertexArrays;
	PFNGLBINDBUFFERPROC					glBindBuffer;
	PFNGLBINDBUFFERRANGEPROC			glBindBufferRange;
	PFNGLBUFFERDATAPROC					glBufferData;
	PFNGLGENVERTEXARRAYSPROC			glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC			glBindVertexArray;
	PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
	PFNGLVERTEXATTRIBIPOINTERPROC		glVertexAttribIPointer;

	PFNGLVERTEXATTRIB3FPROC				glVertexAttrib3f;
	PFNGLVERTEXATTRIB4FPROC				glVertexAttrib4f;

	PFNGLDELETEPROGRAMPROC				glDeleteProgram;
	PFNGLDELETESHADERPROC				glDeleteShader;

	PFNGLCREATESHADERPROC				glCreateShader;
	PFNGLSHADERSOURCEPROC				glShaderSource;
	PFNGLCOMPILESHADERPROC				glCompileShader;
	PFNGLCREATEPROGRAMPROC				glCreateProgram;
	PFNGLATTACHSHADERPROC				glAttachShader;
	PFNGLDETACHSHADERPROC				glDetachShader;
	PFNGLLINKPROGRAMPROC				glLinkProgram;
	PFNGLUSEPROGRAMPROC					glUseProgram;
	PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation;
	PFNGLBINDFRAGDATALOCATIONPROC		glBindFragDataLocation;	
	PFNGLGETFRAGDATALOCATIONPROC		glGetFragDataLocation;
	PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
	PFNGLGETSHADERIVPROC				glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
	PFNGLDRAWBUFFERSPROC				glDrawBuffers;

	PFNGLUNIFORM4FPROC					glUniform4f;
	PFNGLUNIFORM4FVPROC					glUniform4fv;
	PFNGLUNIFORM3FVPROC					glUniform3fv;
	PFNGLUNIFORM2FVPROC					glUniform2fv;
	PFNGLUNIFORM3FPROC					glUniform3f;
	PFNGLUNIFORM2FPROC					glUniform2f;
	PFNGLUNIFORM1FPROC					glUniform1f;
	PFNGLUNIFORM1IPROC					glUniform1i;

	PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv;
	PFNGLUNIFORMMATRIX3FVPROC			glUniformMatrix3fv;

	PFNGLMAPBUFFERPROC					glMapBuffer;
	PFNGLMAPBUFFERRANGEPROC				glMapBufferRange;
	PFNGLUNMAPBUFFERPROC				glUnmapBuffer;

	PFNGLGETPROGRAMIVPROC				glGetProgramiv;
	PFNGLGETACTIVEUNIFORMNAMEPROC		glGetActiveUniformName;
	PFNGLGETACTIVEUNIFORMSIVPROC		glGetActiveUniformsiv;
	PFNGLGETACTIVEUNIFORMPROC			glGetActiveUniform;

	PFNGLDRAWELEMENTSBASEVERTEXPROC		glDrawElementsBaseVertex;

	// Textures

	PFNGLACTIVETEXTUREPROC				glActiveTexture;
	PFNGLGENERATEMIPMAPPROC				glGenerateMipmap;	

	// Buffers

	PFNGLGENRENDERBUFFERSPROC			glGenRenderbuffers;
	PFNGLBINDRENDERBUFFERPROC			glBindRenderbuffer;
	PFNGLRENDERBUFFERSTORAGEPROC		glRenderbufferStorage;

	PFNGLCOPYBUFFERSUBDATAPROC			glCopyBufferSubData;
	PFNGLBUFFERSUBDATAPROC				glBufferSubData;

	PFNGLGENFRAMEBUFFERSPROC			glGenFramebuffers;
	PFNGLDELETERENDERBUFFERSPROC		glDeleteRenderbuffers;
	PFNGLBINDFRAMEBUFFERPROC			glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTURE2DPROC		glFramebufferTexture2D;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC	glFramebufferRenderbuffer;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus;

	PFNGLBLITFRAMEBUFFERPROC			glBlitFramebuffer;
	PFNGLBLENDEQUATIONPROC				glBlendEquation;

	PFNWGLMAKECONTEXTCURRENTARBPROC		wglMakeContextCurrent;
	PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;
	PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB;

	bool IsRealContext() { return m_isRealContext; }

protected:

	bool m_isRealContext;
};

#endif /* YDS_OPENGL_CONTEXT_H */
