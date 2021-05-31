#include "../include/yds_vulkan_windows_context.h"

#include "../include/yds_windows_window.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

ysVulkanWindowsContext::ysVulkanWindowsContext()
    : ysVulkanContext(ysWindowSystemObject::Platform::Windows)
{
    /* void */
}

ysVulkanWindowsContext::~ysVulkanWindowsContext() {
    /* void */
}

ysError ysVulkanWindowsContext::CreateRenderingContext(
    ysVulkanDevice *device,
    ysWindow *window)
{
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (window->GetPlatform() != ysWindow::Platform::Windows) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    const char *extensions[] = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "Delta Studio";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // TODO: enable only for debug builds
    const char *layers[] = { "VK_LAYER_KHRONOS_validation" };

    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = nullptr;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledExtensionCount = sizeof(extensions) / sizeof(const char *);
    instInfo.ppEnabledExtensionNames = extensions;
    instInfo.enabledLayerCount = 1;
    instInfo.ppEnabledLayerNames = layers;

    VkInstance instance;
    vkCreateInstance(&instInfo, nullptr, &instance);

    ysWindowsWindow *win32Window = static_cast<ysWindowsWindow *>(window);

    VkWin32SurfaceCreateInfoKHR win32Info = {};
    win32Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32Info.pNext = nullptr;
    win32Info.flags = 0;
    win32Info.hinstance = win32Window->GetInstance();
    win32Info.hwnd = win32Window->GetWindowHandle();

    VkSurfaceKHR surface;
    VkResult result = vkCreateWin32SurfaceKHR(instance, &win32Info, nullptr, &surface);
    if (result != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}
