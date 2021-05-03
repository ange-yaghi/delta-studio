#include "../include/yds_d3d11_texture.h"

#include "../include/yds_d3d11_utilities.h"

ysD3D11Texture::ysD3D11Texture() : ysTexture(DeviceAPI::DirectX11) {
    m_resourceView = nullptr;
    m_renderTargetView = nullptr;
    m_textureResource = nullptr;
}

ysD3D11Texture::~ysD3D11Texture() {
    /* void */
}

void ysD3D11Texture::SetDebugName(const std::string &debugName) {
    ysTexture::SetDebugName(debugName);

    D3D11SetDebugName(m_resourceView, debugName + "_RESOURCE_VIEW");
    D3D11SetDebugName(m_renderTargetView, debugName + "_RENDER_TARGET_VIEW");
}
