#include "../include/texture_asset.h"

dbasic::TextureAsset::TextureAsset() : ysObject("TextureAsset") {
    m_texture = nullptr;
    m_name = "";
}

dbasic::TextureAsset::~TextureAsset() {
    /* void */
}
