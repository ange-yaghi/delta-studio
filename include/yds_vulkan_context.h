#ifndef YDS_VULKAN_CONTEXT_H
#define YDS_VULKAN_CONTEXT_H

#include "yds_window.h"
#include "yds_rendering_context.h"

#include "yds_vulkan_decl.h"

class ysVulkanContext : public ysRenderingContext {
    friend class ysVulkanDevice;

public:
    ysVulkanContext();
    ysVulkanContext(ysWindowSystemObject::Platform platform);
    virtual ~ysVulkanContext();

    virtual ysError Create(ysVulkanDevice *device, ysWindow *window) = 0;
    virtual ysError Destroy() = 0;

    VkInstance GetInstance() const { return m_instance; }
    VkSurfaceKHR GetSurface() const { return m_surface; }

protected:
    VkInstance m_instance;
    VkSurfaceKHR m_surface;
};

#endif /* YDS_VULKAN_CONTEXT_H */
