#include "../include/yds_vulkan_context.h"

ysVulkanContext::ysVulkanContext()
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    /* void */
}

ysVulkanContext::ysVulkanContext(ysWindowSystemObject::Platform platform)
    : ysRenderingContext(DeviceAPI::Vulkan, ysWindowSystemObject::Platform::Unknown) 
{
    /* void */
}

ysVulkanContext::~ysVulkanContext() {
    /* void */
}
