#include "../include/yds_vulkan_windows_context.h"

#include "../include/yds_windows_window.h"
#include "../include/yds_vulkan_device.h"
#include "../include/yds_vulkan.h"

ysVulkanWindowsContext::ysVulkanWindowsContext()
    : ysVulkanContext(ysWindowSystemObject::Platform::Windows)
{
    /* void */
}

ysVulkanWindowsContext::~ysVulkanWindowsContext() {
    /* void */
}

ysError ysVulkanWindowsContext::Create(
    ysVulkanDevice *device,
    ysWindow *window)
{
    YDS_ERROR_DECLARE("Create");

    if (window->GetPlatform() != ysWindow::Platform::Windows) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    device->SetContext(this);

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

    vkCreateInstance(&instInfo, nullptr, &m_instance);

    ysWindowsWindow *win32Window = static_cast<ysWindowsWindow *>(window);

    VkWin32SurfaceCreateInfoKHR win32Info = {};
    win32Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32Info.pNext = nullptr;
    win32Info.flags = 0;
    win32Info.hinstance = win32Window->GetInstance();
    win32Info.hwnd = win32Window->GetWindowHandle();

    VkResult result = vkCreateWin32SurfaceKHR(m_instance, &win32Info, nullptr, &m_surface);
    if (result != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanWindowsContext::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    m_instance = nullptr;
    m_surface = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
