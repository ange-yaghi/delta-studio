#include "../include/yds_vulkan_context.h"

ysVulkanContext::ysVulkanContext()
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    m_instance = nullptr;
    m_surface = nullptr;
}

ysVulkanContext::ysVulkanContext(ysWindowSystemObject::Platform platform)
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    /* void */
}

ysVulkanContext::~ysVulkanContext() {
    /* void */
}
