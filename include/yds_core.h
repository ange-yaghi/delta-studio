#ifndef YDS_CORE_H
#define YDS_CORE_H

// Base
#include "yds_base.h"
#include "yds_object_data.h"

// Audio
#include "yds_audio_buffer.h"
#include "yds_audio_system.h"

// Input
#include "yds_input_system.h"
#include "yds_keyboard.h"

// Animation
#include "yds_object_animation_data.h"
#include "yds_tool_animation_file.h"

// Timing
#include "yds_time_tag_data.h"
#include "yds_timing.h"

// Math
#include "yds_math.h"

// Memory management
#include "yds_expanding_array.h"

// Textures
#include "yds_texture.h"

// GPU Buffers
#include "yds_gpu_buffer.h"

// Utilities
#include "yds_registry.h"

// Geometry
#include "yds_tool_geometry_file.h"
#include "yds_geometry_preprocessing.h"
#include "yds_geometry_export_file.h"

// Graphics API
#include "yds_device.h"

// OS
#include "yds_window_event_handler.h"

// ========================================================
// Dependencies
// ========================================================

// Main library
#pragma comment(lib, "delta-core.lib")

// External dependencies
#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "windowscodecs.lib")

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_Image.lib")

#pragma comment(lib, "OpenGL32.lib")

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx10d.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "dxguid.lib")

#endif /* YDS_CORE_H */
