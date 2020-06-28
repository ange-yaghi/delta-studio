#include "../include/yds_rendering_context.h"
#include "../include/yds_device.h"

ysRenderingContext::ysRenderingContext() : ysContextObject("RENDERING_CONTEXT", DeviceAPI::Unknown) {
    m_targetWindow = nullptr;
    m_attachedRenderTarget = nullptr;

    m_currentMode = ContextMode::Undefined;

    m_platform = ysWindowSystem::Platform::Unknown;
}

ysRenderingContext::ysRenderingContext(DeviceAPI API, ysWindowSystem::Platform platform) : ysContextObject("RENDERING_CONTEXT", API) {
    m_targetWindow = nullptr;
    m_attachedRenderTarget = nullptr;

    m_currentMode = ContextMode::Undefined;

    m_platform = platform;
}

ysRenderingContext::~ysRenderingContext() {
    /* void */
}
