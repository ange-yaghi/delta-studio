#include "../include/yds_render_geometry_channel.h"

ysRenderGeometryChannel::ysRenderGeometryChannel() : ysObject("RENDER_GEOMETRY_CHANNEL") {
    m_name[0] = '\0';
    m_offset = 0;
    m_format = ChannelFormat::Undefined;
}

ysRenderGeometryChannel::~ysRenderGeometryChannel() {
    /* void */
}
