#pragma once
#include "IRenderer.h"
#include <memory>
#include "Vulkan/VulkanCommon.h"
#include "Vulkan/VulkanRenderer.h"
#include "RVulkan/RVulkanRenderer.h"
//#include "DirectX/D3D12Common.hpp"
#include "DirectX/DirectX12Renderer.h"

std::unique_ptr<IRenderer> CreateRenderer(API api, void* window, PRESENTMODE presentMode, asset::AssetManager& assetManager );

