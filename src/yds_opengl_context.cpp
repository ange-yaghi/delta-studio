#include "../include/yds_opengl_context.h"

#if defined(_WIN64)

ysOpenGLVirtualContext::ysOpenGLVirtualContext() : ysRenderingContext(DeviceAPI::OpenGL4_0, ysWindowSystemObject::Platform::Unknown) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::ysOpenGLVirtualContext(ysWindowSystemObject::Platform platform) : ysRenderingContext(DeviceAPI::OpenGL4_0, platform) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::~ysOpenGLVirtualContext() {
    /* void */
}

#endif /* Windows */
