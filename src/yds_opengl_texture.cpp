#include "../include/yds_opengl_texture.h"

ysOpenGLTexture::ysOpenGLTexture() : ysTexture(DeviceAPI::OpenGL4_0) {
    m_handle = 0;
}

ysOpenGLTexture::~ysOpenGLTexture() {
    /* void */
}
