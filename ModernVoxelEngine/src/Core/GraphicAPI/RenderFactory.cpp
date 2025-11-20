#include "RenderFactory.h"

std::unique_ptr<IRenderer> CreateRenderer(API api, void* windows, PRESENTMODE presentMode, asset::AssetManager& assetManager)
{
	switch (api)
	{
	case API::VULKAN:
		return std::make_unique<vulkan::VulkanRenderer>(windows, presentMode, assetManager);
	case API::OPENGL:
		break;
	case API::DIRECTX:
		return std::make_unique<d3d12::DirectX12Renderer>(windows, presentMode, assetManager);
		break;
	default:
		break;
	}
	return std::unique_ptr<IRenderer>();
}
