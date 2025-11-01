#pragma once
#include "IRenderer.h"
#include <memory>
#include "Vulkan/VulkanCommon.h"
#include "Vulkan/VulkanRenderer.h"
#include "RVulkan/RVulkanRenderer.h"

std::unique_ptr<IRenderer> CreateRenderer(API api, GLFWwindow* window, VkPresentModeKHR presentMode, asset::AssetManager& assetManager );

