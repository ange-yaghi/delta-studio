#include "../include/yds_render_geometry_format.h"

ysRenderGeometryFormat::ysRenderGeometryFormat() : ysObject("RENDER_GEOMETRY_FORMAT") {
    m_formatSize = 0;
}

ysRenderGeometryFormat::~ysRenderGeometryFormat() {
    /* void */
}

int ysRenderGeometryFormat::GetChannelCount() const {
    return m_channels.GetNumObjects();
}

int ysRenderGeometryFormat::GetFormatSize() const {
    return m_formatSize;
}

void ysRenderGeometryFormat::AddChannel(const char *name, int offset, ysRenderGeometryChannel::ChannelFormat format) {
    ysRenderGeometryChannel *newChannel = m_channels.New();
    strcpy_s(newChannel->m_name, ysRenderGeometryChannel::MAX_NAME_LENGTH, name);
    newChannel->m_format = format;
    newChannel->m_offset = offset;

    m_formatSize += newChannel->GetSize();
}
