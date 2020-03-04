#include "../include/yds_opengl_context.h"

ysOpenGLVirtualContext::ysOpenGLVirtualContext() : ysRenderingContext(OPENGL4_0, ysWindowSystemObject::Platform::UNKNOWN) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::ysOpenGLVirtualContext(ysWindowSystemObject::Platform platform) : ysRenderingContext(OPENGL4_0, platform) {
    m_isRealContext = false;
}

ysOpenGLVirtualContext::~ysOpenGLVirtualContext() {
    /* void */
}
