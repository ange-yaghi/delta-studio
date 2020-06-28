#include "../include/yds_opengl_context.h"

ysOpenGLVirtualContext::ysOpenGLVirtualContext() : ysRenderingContext(DeviceAPI::OpenGL4_0, ysWindowSystemObject::Platform::Unknown) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::ysOpenGLVirtualContext(ysWindowSystemObject::Platform platform) : ysRenderingContext(DeviceAPI::OpenGL4_0, platform) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::~ysOpenGLVirtualContext() {
    /* void */
}
