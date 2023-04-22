#ifndef YDS_VULKAN_H
#define YDS_VULKAN_H

#define DELTA_ENABLE_VULKAN 0

#define NOMINMAX
#include <Windows.h>

#if DELTA_ENABLE_VULKAN
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>
#endif

#endif /* YDS_VULKAN_H */
