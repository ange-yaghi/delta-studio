#include "../include/yds_texture.h"

ysTexture::ysTexture() : ysContextObject("TEXTURE", API_UNKNOWN) {
    m_filename[0] = '\0';
    m_width = 0;
    m_height = 0;
}

ysTexture::ysTexture(DEVICE_API API) : ysContextObject("TEXTURE", API) {
    m_filename[0] = '\0';
    m_width = 0;
    m_height = 0;
}

ysTexture::~ysTexture() {
    /* void */
}
