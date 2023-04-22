#include "../include/yds_vulkan_context.h"

ysVulkanContext::ysVulkanContext()
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    m_surface = nullptr;
    m_device = nullptr;
}

ysVulkanContext::ysVulkanContext(ysWindowSystemObject::Platform platform)
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    m_surface = nullptr;
    m_device = nullptr;
}

ysVulkanContext::~ysVulkanContext() {
    /* void */
}

ysError ysVulkanContext::Create(ysVulkanDevice *device, ysWindow *window) {
    YDS_ERROR_DECLARE("Create");

    m_device = device;

    return YDS_ERROR_RETURN(ysError::None);
}
