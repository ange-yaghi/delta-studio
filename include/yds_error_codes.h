#ifndef YDS_ERROR_CODES_H
#define YDS_ERROR_CODES_H

enum class ysError {
    YDS_NO_ERROR,

    // ------------------------------------------------------------------------------
    // Programming Errors
    // ------------------------------------------------------------------------------
    INDEX_PROGRAMMING_ERRORS,
    YDS_NULL_POINTER_FOR_OUTPUT = INDEX_PROGRAMMING_ERRORS,
    YDS_INVALID_PARAMETER,
    YDS_MULTIPLE_ERROR_SYSTEMS, // **
    YDS_MULTIPLE_SYSTEMS,

    YDS_OUT_OF_MEMORY,

    YDS_INVALID_GPU_BUFFER_TYPE,
    YDS_INCOMPATIBLE_PLATFORMS,
    YDS_NO_PLATFORM,

    YDS_INVALID_OPERATION,
    YDS_NOT_IMPLEMENTED,

    YDS_UNINTIALIZED_BUFFER,
    YDS_OUT_OF_BOUNDS,

    // Shaders
    YDS_PROGRAM_ALREADY_LINKED,

    // Contexts
    YDS_CONTEXT_ALREADY_HAS_RENDER_TARGET,

    YDS_NO_DEVICE,
    YDS_NO_RENDER_TARGET,
    YDS_NO_CONTEXT,


    // ------------------------------------------------------------------------------
    // API Errors
    // ------------------------------------------------------------------------------

    INDEX_API_ERRORS,
    YDS_COULD_NOT_CREATE_GRAPHICS_DEVICE = INDEX_API_ERRORS,
    YDS_COULD_NOT_OBTAIN_DEVICE,

    YDS_API_ERROR,

    // DirectX Specific
    YDS_COULD_NOT_CREATE_SWAP_CHAIN,

    YDS_COULD_NOT_ENTER_FULLSCREEN,
    YDS_COULD_NOT_EXIT_FULLSCREEN,

    YDS_COULD_NOT_CREATE_GPU_BUFFER,

    // OpenGL specific
    YDS_COULD_NOT_ACTIVATE_TEMPORARY_CONTEXT,
    YDS_COULD_NOT_ACTIVATE_CONTEXT,
    YDS_COULD_NOT_CREATE_TEMPORARY_CONTEXT,
    YDS_COULD_NOT_CREATE_CONTEXT,
    YDS_COULD_NOT_DESTROY_CONTEXT,
    YDS_BUFFER_SWAP_ERROR,

    // Render Targets
    YDS_COULD_NOT_GET_BACK_BUFFER,
    YDS_COULD_NOT_CREATE_RENDER_TARGET,
    YDS_COULD_NOT_CREATE_DEPTH_BUFFER,

    // Shaders
    YDS_VERTEX_SHADER_COMPILATION_ERROR,
    YDS_FRAGMENT_SHADER_COMPILATION_ERROR,
    YDS_COULD_NOT_CREATE_SHADER,

    // Input Formats
    YDS_INCOMPATIBLE_INPUT_FORMAT,

    // Textures
    YDS_COULD_NOT_OPEN_TEXTURE,
    YDS_COULD_NOT_MAKE_SHADER_RESOURCE_VIEW,

    // Testing
    INDEX_TEST_ERRORS,
    YDS_TEST_ERROR = INDEX_TEST_ERRORS,

    // Input System
    YDS_NO_DEVICE_LIST,
    YDS_COULD_NOT_REGISTER_FOR_INPUT,
    YDS_NO_WINDOW_SYSTEM,

    // Audio System
    YDS_BUFFER_ALREADY_LOCKED,
    YDS_BUFFER_NOT_LOCKED,

    // Streaming Audio
    YDS_NO_FILE,
    YDS_NO_AUDIO_BUFFER,
    YDS_NO_FILE_BUFFER,
    YDS_INCOMPATIBLE_BUFFER_AND_FILE,

    // ------------------------------------------------------------------------------
    // File Errors
    // ------------------------------------------------------------------------------

    YDS_COULD_NOT_OPEN_FILE,
    YDS_INVALID_FILE_TYPE,
    YDS_UNSUPPORTED_FILE_VERSION,
    YDS_CORRUPTED_FILE,

    YDS_UNSUPPORTED_TYPE,
};

#endif /* YDS_ERROR_CODES_H */
