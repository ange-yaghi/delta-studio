#include "../include/yds_rendering_context.h"
#include "../include/yds_device.h"

ysRenderingContext::ysRenderingContext() : ysContextObject("RENDERING_CONTEXT", API_UNKNOWN) {
    m_targetWindow = NULL;
    m_attachedRenderTarget = NULL;

    m_currentMode = ContextMode::UNDEFINED;

    m_platform = ysWindowSystem::Platform::UNKNOWN;
}

ysRenderingContext::ysRenderingContext(DEVICE_API API, ysWindowSystem::Platform platform) : ysContextObject("RENDERING_CONTEXT", API) {
    m_targetWindow = NULL;
    m_attachedRenderTarget = NULL;

    m_currentMode = ContextMode::UNDEFINED;

    m_platform = platform;
}

ysRenderingContext::~ysRenderingContext() {
    /* void */
}
