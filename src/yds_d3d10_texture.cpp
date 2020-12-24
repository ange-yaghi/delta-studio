#include "../include/yds_d3d10_texture.h"

ysD3D10Texture::ysD3D10Texture() : ysTexture(DeviceAPI::DirectX10) {
    m_resourceView = NULL;
    m_renderTargetView = NULL;
}

ysD3D10Texture::~ysD3D10Texture() {
    /* void */
}
