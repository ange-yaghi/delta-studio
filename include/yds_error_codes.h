#ifndef YDS_ERROR_CODES_H
#define YDS_ERROR_CODES_H

// TODO: Create cases specific to Apple frameworks

enum class ysError {
    None,

    // ------------------------------------------------------------------------------
    // Programming Errors
    // ------------------------------------------------------------------------------

    InvalidParameter,
    MultipleErrorSystems,
    MultipleSystems,

    OutOfMemory,

    InvalidGpuBufferType,
    IncompatiblePlatforms,
    NoPlatform,

    InvalidOperation,
    NotImplemented,

    UninitializedBuffer,
    OutOfBounds,

    // Shaders
    ProgramAlreadyLinked,
    ProgramNotLinked,
    ProgramLinkError,

    // Contexts
    ContextAlreadyHasRenderTarget,

    NoDevice,
    NoRenderTarget,
    NoContext,

    // ------------------------------------------------------------------------------
    // API Errors
    // ------------------------------------------------------------------------------

    CouldNotCreateGraphicsDevice,
    CouldNotObtainDevice,

    ApiError,

    // DirectX Specific
    CouldNotCreateSwapChain,

    CouldNotEnterFullscreen,
    CouldNotExitFullscreen,

    CouldNotCreateGpuBuffer,

    // OpenGL specific
    CouldNotActivateTemporaryContext,
    CouldNotActivateContext,
    CouldNotCreateTemporaryContext,
    CouldNotCreateContext,
    CouldNotDestroyContext,
    BufferSwapError,

    // Vulkan specific
    NoQueueFamilyFound,

    // Render Targets
    CouldNotGetBackBuffer,
    CouldNotCreateRenderTarget,
    CouldNotCreateDepthBuffer,

    // Shaders
    VertexShaderCompilationError,
    FragmentShaderCompilationError,
    CouldNotCreateShader, 

    // Input Formats
    IncompatibleInputFormat,

    // Textures
    CouldNotOpenTexture,
    CouldNotMakeShaderResourceView,

    // Testing
    TestError,

    // Input System
    NoDeviceList,
    CouldNotRegisterForInput,
    NoWindowSystem,

    // Audio System
    BufferAlreadyLocked,
    BufferNotLocked,

    // Streaming Audio
    NoFile,
    NoAudioBuffer,
    NoFileBuffer,
    IncompatibleBufferAndFile,

    // ------------------------------------------------------------------------------
    // File Errors
    // ------------------------------------------------------------------------------

    CouldNotOpenFile,
    InvalidFileType,
    UnsupportedFileVersion,
    CorruptedFile,

    UnsupportedType,

    CouldNotCreateSamplerState,
    Unsupported,

    CouldNotEnumerateMonitors,
    CouldNotEnumerateAudioDevices,
    CouldNotCreateDS8Device,
    CouldNotSetDeviceCooperativeLevel,
    CouldNotCreateSoundBuffer,
    NoAudioDevice,
    CouldNotCreateDS8DeviceInvalidParam,
    CouldNotCreateDS8DeviceNoDriver,
    CouldNotCreateDS8DeviceOther,

    CouldNotLoadCompiledShader,

    CouldNotCreateDS8DeviceDeviceInUse,
    CouldNotCreateDS8DeviceNoAggregation,
    CouldNotCreateDS8DeviceOutOfMemory,
};

#endif /* YDS_ERROR_CODES_H */
