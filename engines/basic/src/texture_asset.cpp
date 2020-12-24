#include "../include/texture_asset.h"

dbasic::TextureAsset::TextureAsset() : ysObject("TextureAsset") {
    m_texture = nullptr;
    m_name = "";
}

dbasic::TextureAsset::~TextureAsset() {
    /* void */
}

ysError dbasic::TextureAsset::Destroy(ysDevice *device) {
    YDS_ERROR_DECLARE("Destroy");

    if (m_texture != nullptr) {
        device->DestroyTexture(m_texture);
    }

    return YDS_ERROR_RETURN(ysError::None);
}
