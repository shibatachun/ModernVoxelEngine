#pragma once

#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>
#include "../RHIResource.h"

namespace rvulkan {
	struct Allocator;

}
