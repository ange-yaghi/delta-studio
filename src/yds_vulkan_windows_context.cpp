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
    YDS_NESTED_ERROR_CALL(ysVulkanContext::Create(device, window));

    if (window->GetPlatform() != ysWindow::Platform::Windows) {
        return YDS_ERROR_RETURN(ysError::IncompatiblePlatforms);
    }

    ysWindowsWindow *win32Window = static_cast<ysWindowsWindow *>(window);

    VkWin32SurfaceCreateInfoKHR win32Info = {};
    win32Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32Info.pNext = nullptr;
    win32Info.flags = 0;
    win32Info.hinstance = win32Window->GetInstance();
    win32Info.hwnd = win32Window->GetWindowHandle();

    VkResult result = vkCreateWin32SurfaceKHR(device->GetInstance(), &win32Info, nullptr, &m_surface);
    if (result != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanWindowsContext::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    vkDestroySurfaceKHR(m_device->GetInstance(), m_surface, nullptr);

    m_surface = nullptr;

    return YDS_ERROR_RETURN(ysError::None);
}
