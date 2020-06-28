#include "../include/yds_texture.h"

ysTexture::ysTexture() : ysContextObject("TEXTURE", DeviceAPI::Unknown) {
    m_filename[0] = '\0';
    m_width = 0;
    m_height = 0;
}

ysTexture::ysTexture(DeviceAPI API) : ysContextObject("TEXTURE", API) {
    m_filename[0] = '\0';
    m_width = 0;
    m_height = 0;
}

ysTexture::~ysTexture() {
    /* void */
}
