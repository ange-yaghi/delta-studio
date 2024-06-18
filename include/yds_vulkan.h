#ifndef YDS_VULKAN_H
#define YDS_VULKAN_H

#define DELTA_ENABLE_VULKAN 0

#define NOMINMAX

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include "win32/window.h"
#elif defined(_WIN64)
    #include <Windows.h>
#endif

#if DELTA_ENABLE_VULKAN
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>
#endif

#endif /* YDS_VULKAN_H */
