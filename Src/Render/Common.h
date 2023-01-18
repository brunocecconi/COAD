
#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include "Core/Common.h"

#if PLATFORM_WINDOWS

#ifndef OPENGL_ENABLED
#define OPENGL_ENABLED 0
#endif

#ifndef VULKAN_ENABLED
#define VULKAN_ENABLED 0
#endif

#ifndef D3D11_ENABLED
#define D3D11_ENABLED 0
#endif

#ifndef D3D12_ENABLED
#define D3D12_ENABLED 0
#endif

#if OPENGL_ENABLED
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wgl.h>
#elif VULKAN_ENABLED
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <shaderc/shaderc.h>
#endif

#if !(OPENGL_ENABLED || VULKAN_ENABLED || D3D11_ENABLED || D3D12_ENABLED)
#error No graphics api has been enabled.
#endif

#endif

#endif
