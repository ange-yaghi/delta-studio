#include "../include/yds_vulkan_device.h"

#include "../include/yds_vulkan_render_target.h"
#include "../include/yds_vulkan_context.h"
#include "../include/yds_vulkan_windows_context.h"
#include "../include/yds_vulkan_texture.h"
#include "../include/yds_vulkan.h"

#include <vector>

ysVulkanDevice::ysVulkanDevice() {
    m_device = nullptr;
    m_instance = nullptr;
}

ysVulkanDevice::~ysVulkanDevice() {
    /* void */
}

ysError ysVulkanDevice::InitializeDevice() {
    YDS_ERROR_DECLARE("InitializeDevice");

    YDS_NESTED_ERROR_CALL(CreateVulkanInstance());

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::DestroyDevice() {
    YDS_ERROR_DECLARE("DestroyDevice");

    vkDestroyInstance(m_instance, nullptr);

    return YDS_ERROR_RETURN(ysError::None);
}

bool ysVulkanDevice::CheckSupport() {
    return false;
}

ysError ysVulkanDevice::CreateRenderingContext(ysRenderingContext **renderingContext, ysWindow *window) {
    YDS_ERROR_DECLARE("CreateRenderingContext");

    if (renderingContext == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *renderingContext = nullptr;

    if (window->GetPlatform() == ysWindowSystemObject::Platform::Windows) {
        ysVulkanWindowsContext *newContext = m_renderingContexts.NewGeneric<ysVulkanWindowsContext>();
        newContext->m_targetWindow = window;
        *renderingContext = static_cast<ysRenderingContext *>(newContext);

        YDS_NESTED_ERROR_CALL(newContext->Create(this, window));

        if (m_device == nullptr) {
            YDS_NESTED_ERROR_CALL(CreateVulkanDevice(newContext->GetSurface()));
        }

        return YDS_ERROR_RETURN(ysError::None);
    }
    else {
        return YDS_ERROR_RETURN_MSG(ysError::IncompatiblePlatforms, "Only Windows platforms are currently supported.");
    }
}

ysError ysVulkanDevice::UpdateRenderingContext(ysRenderingContext *context) {
    YDS_ERROR_DECLARE("UpdateRenderingContext");

    /* TODO */

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::DestroyRenderingContext(ysRenderingContext *&context) {
    YDS_ERROR_DECLARE("DestroyRenderingContext");

    ysVulkanContext *vulkanContext = static_cast<ysVulkanContext *>(context);
    YDS_NESTED_ERROR_CALL(vulkanContext->Destroy());

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::SetContextMode(ysRenderingContext *context, ysRenderingContext::ContextMode mode) {
    return ysError();
}

ysError ysVulkanDevice::CreateOnScreenRenderTarget(ysRenderTarget **newTarget, ysRenderingContext *context, bool depthBuffer) {
    YDS_ERROR_DECLARE("CreateOnScreenRenderTarget");

    if (newTarget == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTarget = nullptr;

    if (context == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    if (context->GetAttachedRenderTarget() != nullptr) return YDS_ERROR_RETURN(ysError::ContextAlreadyHasRenderTarget);

    ysVulkanRenderTarget *newRenderTarget = m_renderTargets.NewGeneric<ysVulkanRenderTarget>();
    ysVulkanContext *vulkanContext = static_cast<ysVulkanContext *>(context);

    newRenderTarget->m_type = ysRenderTarget::Type::OnScreen;
    newRenderTarget->m_posX = 0;
    newRenderTarget->m_posY = 0;
    newRenderTarget->m_width = context->GetWindow()->GetGameWidth();
    newRenderTarget->m_height = context->GetWindow()->GetGameHeight();
    newRenderTarget->m_physicalWidth = context->GetWindow()->GetScreenWidth();
    newRenderTarget->m_physicalHeight = context->GetWindow()->GetScreenHeight();
    newRenderTarget->m_format = ysRenderTarget::Format::R8G8B8A8_UNORM;
    newRenderTarget->m_hasDepthBuffer = depthBuffer;
    newRenderTarget->m_associatedContext = context;

    *newTarget = newRenderTarget;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::CreateOffScreenRenderTarget(ysRenderTarget **newTarget, int width, int height, ysRenderTarget::Format format, bool colorData, bool depthBuffer) {
    return ysError();
}

ysError ysVulkanDevice::CreateSubRenderTarget(ysRenderTarget **newTarget, ysRenderTarget *parent, int x, int y, int width, int height) {
    return ysError();
}

ysError ysVulkanDevice::ResizeRenderTarget(ysRenderTarget *target, int width, int height, int pwidth, int pheight) {
    return ysError();
}

ysError ysVulkanDevice::DestroyRenderTarget(ysRenderTarget *&target) {
    return ysError();
}

ysError ysVulkanDevice::SetRenderTarget(ysRenderTarget *target) {
    return ysError();
}

ysError ysVulkanDevice::SetDepthTestEnabled(ysRenderTarget *target, bool enable) {
    return ysError();
}

ysError ysVulkanDevice::ReadRenderTarget(ysRenderTarget *src, uint8_t *target) {
    return ysError();
}

ysError ysVulkanDevice::ClearBuffers(const float *clearColor) {
    return ysError();
}

ysError ysVulkanDevice::Present() {
    return ysError();
}

ysError ysVulkanDevice::SetFaceCulling(bool faceCulling) {
    return ysError();
}

ysError ysVulkanDevice::SetFaceCullingMode(CullMode cullMode) {
    return ysError();
}

ysError ysVulkanDevice::CreateVertexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    return ysError();
}

ysError ysVulkanDevice::CreateIndexBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    return ysError();
}

ysError ysVulkanDevice::CreateConstantBuffer(ysGPUBuffer **newBuffer, int size, char *data, bool mirrorToRam) {
    return ysError();
}

ysError ysVulkanDevice::UseVertexBuffer(ysGPUBuffer *buffer, int stride, int offset) {
    return ysError();
}

ysError ysVulkanDevice::UseIndexBuffer(ysGPUBuffer *buffer, int offset) {
    return ysError();
}

ysError ysVulkanDevice::UseConstantBuffer(ysGPUBuffer *buffer, int slot) {
    return ysError();
}

ysError ysVulkanDevice::EditBufferDataRange(ysGPUBuffer *buffer, char *data, int size, int offset) {
    return ysError();
}

ysError ysVulkanDevice::EditBufferData(ysGPUBuffer *buffer, char *data) {
    return ysError();
}

ysError ysVulkanDevice::DestroyGPUBuffer(ysGPUBuffer *&buffer) {
    return ysError();
}

ysError ysVulkanDevice::CreateVertexShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    return ysError();
}

ysError ysVulkanDevice::CreatePixelShader(ysShader **newShader, const char *shaderFilename, const char *shaderName) {
    return ysError();
}

ysError ysVulkanDevice::DestroyShader(ysShader *&shader) {
    return ysError();
}

ysError ysVulkanDevice::CreateShaderProgram(ysShaderProgram **newProgram) {
    return ysError();
}

ysError ysVulkanDevice::DestroyShaderProgram(ysShaderProgram *&shader, bool destroyShaders) {
    return ysError();
}

ysError ysVulkanDevice::AttachShader(ysShaderProgram *targetProgram, ysShader *shader) {
    return ysError();
}

ysError ysVulkanDevice::LinkProgram(ysShaderProgram *program) {
    return ysError();
}

ysError ysVulkanDevice::UseShaderProgram(ysShaderProgram *) {
    return ysError();
}

ysError ysVulkanDevice::CreateInputLayout(ysInputLayout **newLayout, ysShader *shader, const ysRenderGeometryFormat *format) {
    return ysError();
}

ysError ysVulkanDevice::UseInputLayout(ysInputLayout *layout) {
    return ysError();
}

ysError ysVulkanDevice::DestroyInputLayout(ysInputLayout *&layout) {
    return ysError();
}

ysError ysVulkanDevice::CreateTexture(ysTexture **texture, const char *fname) {
    return ysError();
}

ysError ysVulkanDevice::CreateTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    return ysError();
}

ysError ysVulkanDevice::UpdateTexture(ysTexture *texture, const unsigned char *buffer) {
    return ysError();
}

ysError ysVulkanDevice::CreateAlphaTexture(ysTexture **texture, int width, int height, const unsigned char *buffer) {
    YDS_ERROR_DECLARE("CreateTexture");

    if (texture == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *texture = nullptr;

    ysVulkanTexture *newTexture = m_textures.NewGeneric<ysVulkanTexture>();
    strcpy_s(newTexture->m_filename, 257, "");
    newTexture->m_width = width;
    newTexture->m_height = height;

    *texture = static_cast<ysTexture *>(newTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::DestroyTexture(ysTexture *&texture) {
    return ysError();
}

ysError ysVulkanDevice::UseTexture(ysTexture *texture, int slot) {
    return ysError();
}

ysError ysVulkanDevice::UseRenderTargetAsTexture(ysRenderTarget *renderTarget, int slot) {
    return ysError();
}

void ysVulkanDevice::Draw(int numFaces, int indexOffset, int vertexOffset) {
}

ysError ysVulkanDevice::CreateVulkanInstance() {
    YDS_ERROR_DECLARE("CreateVulkanInstance");

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

    if (vkCreateInstance(&instInfo, nullptr, &m_instance) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysVulkanDevice::CreateVulkanDevice(VkSurfaceKHR surface) {
    YDS_ERROR_DECLARE("CreateVulkanDevice");

    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    if (deviceCount < 1) {
        return YDS_ERROR_RETURN(ysError::CouldNotObtainDevice);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount, nullptr);
    if (vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::CouldNotObtainDevice);
    }

    VkPhysicalDevice device = devices.front();

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    if (queueFamilyCount < 1) {
        return YDS_ERROR_RETURN(ysError::NoQueueFamilyFound);
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

    uint32_t deviceExtensionCount = 0;
    if (vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &deviceExtensionCount,
        nullptr) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    if (vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &deviceExtensionCount,
        deviceExtensions.data()) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    int graphicsQueueIndex = -1;
    for (int i = 0; i < queueFamilyCount; ++i) {
        VkBool32 supportsPresent;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent);

        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supportsPresent) {
            graphicsQueueIndex = i;
            break;
        }
    }

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueInfo.queueFamilyIndex = graphicsQueueIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    std::vector<const char *> requiredExtensions = { "VK_KHR_swapchain" };

    VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
    deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateDevice(device, &deviceInfo, nullptr, &m_device) != VkResult::VK_SUCCESS) {
        return YDS_ERROR_RETURN(ysError::ApiError);
    }

    return YDS_ERROR_RETURN(ysError::None);
}
