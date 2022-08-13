#ifndef YDS_OPENGL_SDL_CONTEXT_H
#define YDS_OPENGL_SDL_CONTEXT_H

#include "yds_opengl_context.h"

#include "OpenGL.h"

class ysOpenGLDevice;

class ysOpenGLSDLContext : public ysOpenGLVirtualContext {
    friend ysOpenGLDevice;

public:
    ysOpenGLSDLContext();
    virtual ~ysOpenGLSDLContext();

    ysError CreateRenderingContext(ysOpenGLDevice *device, ysWindow *window, int major, int minor);

    virtual ysError DestroyContext();
    virtual ysError TransferContext(ysOpenGLVirtualContext *context);
    virtual ysError SetContextMode(ContextMode mode);
    virtual ysError SetContext(ysRenderingContext *realContext);
    virtual ysError Present();

protected:
    void *m_context = nullptr;

    ysOpenGLDevice *m_device;

protected:
    void LoadAllExtensions();
};

#endif /* YDS_OPENGL_WINDOWS_CONTEXT_H */
