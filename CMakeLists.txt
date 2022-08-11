cmake_minimum_required(VERSION 3.10)

# Enable group projects in folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

project(delta-studio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (MSVC)
    add_definitions("-DMSVC")
else()
    # This codebase uses a bunch of msvc extentsions, and it's easier just to ask
    # other compilers to just support as many of those extentions as possible
    # At least for now.
    add_compile_options(-fms-extensions)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TARGET_ARCHITECTURE x64)
else()
    set(TARGET_ARCHITECTURE x86)
endif()

# =========================================================
# D3DX

set(
    D3DX_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libraries/D3DX/lib/${TARGET_ARCHITECTURE})
set(
    DXGI_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libraries/DXGI/lib/${TARGET_ARCHITECTURE})
include_directories(dependencies/libraries/D3DX/include)

# =========================================================
# DirectSound

set(
    DIRECT_SOUND_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libraries/DirectSound/lib/${TARGET_ARCHITECTURE})

# =========================================================
# OpenGL

include_directories(dependencies/libraries/OpenGL/include)

# =========================================================
# Vulkan

set(
    VULKAN_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libraries/vulkan/lib/${TARGET_ARCHITECTURE})
include_directories(dependencies/libraries/vulkan/include)

# =========================================================
# SDL2

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# =========================================================
# Boost

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.40 COMPONENTS filesystem REQUIRED)

# =========================================================
# STB

include_directories(dependencies/libraries/stb)

add_library(delta-core STATIC
    # Source files
    src/yds_animation_action.cpp
    src/yds_animation_action_binding.cpp
    src/yds_animation_curve.cpp
    src/yds_animation_interchange_file.cpp
    src/yds_animation_interchange_file_reader.cpp
    src/yds_animation_interchange_file_reader_0_0.cpp
    src/yds_animation_interchange_file_reader_0_1.cpp
    src/yds_animation_mixer.cpp
    src/yds_animation_target.cpp
    src/yds_audio_buffer.cpp
    src/yds_audio_device.cpp
    src/yds_audio_file.cpp
    src/yds_audio_parameters.cpp
    src/yds_audio_source.cpp
    src/yds_audio_streaming.cpp
    src/yds_audio_system.cpp
    src/yds_audio_system_object.cpp
    src/yds_base.cpp
    src/yds_breakdown_timer.cpp
    src/yds_breakdown_timer_channel.cpp
    src/yds_color.cpp
    src/yds_context_object.cpp
    src/yds_d3d10_context.cpp
    src/yds_d3d10_device.cpp
    src/yds_d3d10_gpu_buffer.cpp
    src/yds_d3d10_input_layout.cpp
    src/yds_d3d10_render_target.cpp
    src/yds_d3d10_shader.cpp
    src/yds_d3d10_shader_program.cpp
    src/yds_d3d10_texture.cpp
    src/yds_d3d11_context.cpp
    src/yds_d3d11_device.cpp
    src/yds_d3d11_gpu_buffer.cpp
    src/yds_d3d11_input_layout.cpp
    src/yds_d3d11_render_target.cpp
    src/yds_d3d11_shader.cpp
    src/yds_d3d11_shader_program.cpp
    src/yds_d3d11_texture.cpp
    src/yds_d3d11_utilities.cpp
    src/yds_depth_buffer.cpp
    src/yds_device.cpp
    src/yds_ds8_audio_buffer.cpp
    src/yds_ds8_audio_source.cpp
    src/yds_ds8_device.cpp
    src/yds_ds8_system.cpp
    src/yds_dynamic_allocator.cpp
    src/yds_error_handler.cpp
    src/yds_error_system.cpp
    src/yds_file.cpp
    src/yds_file_logger.cpp
    src/yds_geometry_export_file.cpp
    src/yds_geometry_preprocessing.cpp
    src/yds_gpu_buffer.cpp
    src/yds_input_device.cpp
    src/yds_input_layout.cpp
    src/yds_input_system.cpp
    src/yds_interchange_file_0_0.cpp
    src/yds_interchange_file_0_1.cpp
    src/yds_interchange_object.cpp
    src/yds_keyboard.cpp
    src/yds_keyboard_aggregator.cpp
    src/yds_linked_list.cpp
    src/yds_logger.cpp
    src/yds_logger_output.cpp
    src/yds_math.cpp
    src/yds_memory_base.cpp
    src/yds_monitor.cpp
    src/yds_mouse.cpp
    src/yds_mouse_aggregator.cpp
    src/yds_object_animation_data.cpp
    src/yds_object_data.cpp
    src/yds_opengl_context.cpp
    src/yds_opengl_device.cpp
    src/yds_opengl_gpu_buffer.cpp
    src/yds_opengl_input_layout.cpp
    src/yds_opengl_render_target.cpp
    src/yds_opengl_shader.cpp
    src/yds_opengl_shader_program.cpp
    src/yds_opengl_texture.cpp
    src/yds_opengl_windows_context.cpp
    src/yds_rendering_context.cpp
    src/yds_render_geometry_channel.cpp
    src/yds_render_geometry_format.cpp
    src/yds_render_target.cpp
    src/yds_shader.cpp
    src/yds_shader_program.cpp
    src/yds_stat.cpp
    src/yds_texture.cpp
    src/yds_time_tag_data.cpp
    src/yds_timing.cpp
    src/yds_tool_animation_file.cpp
    src/yds_tool_geometry_file.cpp
    src/yds_transform.cpp
    src/yds_vulkan_context.cpp
    src/yds_vulkan_device.cpp
    src/yds_vulkan_gpu_buffer.cpp
    src/yds_vulkan_input_layout.cpp
    src/yds_vulkan_render_target.cpp
    src/yds_vulkan_shader.cpp
    src/yds_vulkan_shader_program.cpp
    src/yds_vulkan_texture.cpp
    src/yds_vulkan_windows_context.cpp
    src/yds_window.cpp
    src/yds_windows_audio_wave_file.cpp
    src/yds_windows_input_device.cpp
    src/yds_windows_input_system.cpp
    src/yds_windows_key_maps.cpp
    src/yds_windows_monitor.cpp
    src/yds_windows_window.cpp
    src/yds_windows_window_system.cpp
    src/yds_window_event_handler.cpp
    src/yds_window_system.cpp
    src/yds_window_system_object.cpp

    # Include files
    include/yds_allocator.h
    include/yds_animation_action.h
    include/yds_animation_action_binding.h
    include/yds_animation_curve.h
    include/yds_animation_interchange_file.h
    include/yds_animation_interchange_file_reader.h
    include/yds_animation_interchange_file_reader_0_0.h
    include/yds_animation_interchange_file_reader_0_1.h
    include/yds_animation_mixer.h
    include/yds_animation_target.h
    include/yds_audio_buffer.h
    include/yds_audio_device.h
    include/yds_audio_file.h
    include/yds_audio_parameters.h
    include/yds_audio_source.h
    include/yds_audio_streaming.h
    include/yds_audio_system.h
    include/yds_audio_system_object.h
    include/yds_base.h
    include/yds_breakdown_timer.h
    include/yds_breakdown_timer_channel.h
    include/yds_color.h
    include/yds_context_object.h
    include/yds_core.h
    include/yds_d3d10_context.h
    include/yds_d3d10_device.h
    include/yds_d3d10_gpu_buffer.h
    include/yds_d3d10_input_layout.h
    include/yds_d3d10_render_target.h
    include/yds_d3d10_shader.h
    include/yds_d3d10_shader_program.h
    include/yds_d3d10_texture.h
    include/yds_d3d11_context.h
    include/yds_d3d11_device.h
    include/yds_d3d11_gpu_buffer.h
    include/yds_d3d11_input_layout.h
    include/yds_d3d11_render_target.h
    include/yds_d3d11_shader.h
    include/yds_d3d11_shader_program.h
    include/yds_d3d11_texture.h
    include/yds_d3d11_utilities.h
    include/yds_depth_buffer.h
    include/yds_device.h
    include/yds_ds8_audio_buffer.h
    include/yds_ds8_audio_source.h
    include/yds_ds8_device.h
    include/yds_ds8_system.h
    include/yds_dynamic_allocator.h
    include/yds_dynamic_array.h
    include/yds_dynamic_array_deprecated.h
    include/yds_error_codes.h
    include/yds_error_handler.h
    include/yds_error_system.h
    include/yds_expanding_array.h
    include/yds_file.h
    include/yds_file_logger.h
    include/yds_file_utilities.h
    include/yds_geometry_export_file.h
    include/yds_geometry_preprocessing.h
    include/yds_gpu_buffer.h
    include/yds_handle_array.h
    include/yds_input_device.h
    include/yds_input_layout.h
    include/yds_input_system.h
    include/yds_interchange_file_0_0.h
    include/yds_interchange_file_0_1.h
    include/yds_interchange_object.h
    include/yds_keyboard.h
    include/yds_keyboard_aggregator.h
    include/yds_key_maps.h
    include/yds_linked_list.h
    include/yds_logger.h
    include/yds_logger_output.h
    include/yds_math.h
    include/yds_memory_base.h
    include/yds_monitor.h
    include/yds_mouse.h
    include/yds_mouse_aggregator.h
    include/yds_object_animation_data.h
    include/yds_object_data.h
    include/yds_opengl_context.h
    include/yds_opengl_device.h
    include/yds_opengl_gpu_buffer.h
    include/yds_opengl_input_layout.h
    include/yds_opengl_render_target.h
    include/yds_opengl_shader.h
    include/yds_opengl_shader_program.h
    include/yds_opengl_texture.h
    include/yds_opengl_windows_context.h
    include/yds_queue.h
    include/yds_registry.h
    include/yds_rendering_context.h
    include/yds_render_geometry_channel.h
    include/yds_render_geometry_format.h
    include/yds_render_target.h
    include/yds_shader.h
    include/yds_shader_program.h
    include/yds_stat.h
    include/yds_texture.h
    include/yds_time_tag_data.h
    include/yds_timing.h
    include/yds_tool_animation_file.h
    include/yds_tool_geometry_file.h
    include/yds_transform.h
    include/yds_vulkan.h
    include/yds_vulkan_context.h
    include/yds_vulkan_decl.h
    include/yds_vulkan_device.h
    include/yds_vulkan_gpu_buffer.h
    include/yds_vulkan_input_layout.h
    include/yds_vulkan_render_target.h
    include/yds_vulkan_shader.h
    include/yds_vulkan_shader_program.h
    include/yds_vulkan_texture.h
    include/yds_vulkan_windows_context.h
    include/yds_window.h
    include/yds_windows_audio_wave_file.h
    include/yds_windows_input_device.h
    include/yds_windows_input_system.h
    include/yds_windows_monitor.h
    include/yds_windows_window.h
    include/yds_windows_window_system.h
    include/yds_window_event_handler.h
    include/yds_window_system.h
    include/yds_window_system_object.h
)


target_link_libraries(delta-core PUBLIC
    # SDL
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}

    # D3DX
    d3d9
    d3d10
    d3d11
    ${D3DX_LIBS}/d3dx9.lib
    ${D3DX_LIBS}/d3dx9d.lib
    ${D3DX_LIBS}/d3dx10d.lib
    ${D3DX_LIBS}/d3dx10.lib
    ${D3DX_LIBS}/d3dx11.lib
    ${D3DX_LIBS}/d3dx11d.lib
    ${D3DX_LIBS}/d3dxof.lib

    # DXGI
    ${DXGI_LIBS}/DxErr.lib
    ${DXGI_LIBS}/dxgi.lib
    ${DXGI_LIBS}/dxguid.lib

    # DirectSound
    ${DIRECT_SOUND_LIBS}/dsound.lib

    # Vulkan
    ${VULKAN_LIBS}/vulkan-1.lib

    # Boost
    Boost::filesystem

    winmm
    OpenGL32
)

add_subdirectory(physics)
add_subdirectory(engines)
add_subdirectory(demos)
