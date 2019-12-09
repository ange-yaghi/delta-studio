#include "../include/yds_d3d11_texture.h"

ysD3D11Texture::ysD3D11Texture() : ysTexture(DIRECTX11) {
	m_resourceView = NULL;
	m_renderTargetView = NULL;
}

ysD3D11Texture::~ysD3D11Texture() {
    /* void */
}
