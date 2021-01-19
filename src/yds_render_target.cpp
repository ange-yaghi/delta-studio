#include "../include/yds_render_target.h"

ysRenderTarget::ysRenderTarget() : ysContextObject("RENDER_TARGET", DeviceAPI::Unknown) {
    m_format = Format::R32G32B32_FLOAT;
    m_type = Type::Undefined;
    m_hasColorData = true;
    m_hasDepthBuffer = false;
    m_physicalWidth = 0;
    m_physicalHeight = 0;
    m_width = 0;
    m_height = 0;
    m_posX = 0;
    m_posY = 0;
    m_sampleCount = 1;
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
    m_depthTestEnabled = false;
}

ysRenderTarget::ysRenderTarget(DeviceAPI API) : ysContextObject("RENDER_TARGET", API) {
    m_format = Format::R32G32B32_FLOAT;
    m_type = Type::Undefined;
    m_hasColorData = true;
    m_hasDepthBuffer = false;
    m_physicalWidth = 0;
    m_physicalHeight = 0;
    m_width = 0;
    m_height = 0;
    m_posX = 0;
    m_posY = 0;
    m_sampleCount = 1;
    m_associatedContext = nullptr;
    m_depthBuffer = nullptr;
    m_parent = nullptr;
    m_depthTestEnabled = false;
}

ysRenderTarget::~ysRenderTarget() {
    /* void */
}
