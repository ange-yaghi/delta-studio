#ifndef YDS_VULKAN_CONTEXT_H
#define YDS_VULKAN_CONTEXT_H

#include "yds_window.h"
#include "yds_rendering_context.h"

#include <OpenGL.h>

class ysVulkanContext : public ysRenderingContext {
    friend class ysVulkanDevice;

public:
    ysVulkanContext();
    ysVulkanContext(ysWindowSystemObject::Platform platform);
    virtual ~ysVulkanContext();
};

#endif /* YDS_VULKAN_CONTEXT_H */
