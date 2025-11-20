#include "ModernEngineApplication.h"

void ModernEngineApplication::Init()
{
	InitAssetManager();
	InitRendererBackend(_graphicApi);

}

void ModernEngineApplication::InitRendererBackend(API api)
{
	switch (api)
	{
	case API::VULKAN:
		_render = CreateRenderer(API::VULKAN, _window, PRESENTMODE::IMMEDIATE, *_assetManager);
		break;
	case API::RVULKAN:
		rvulkan::init();
		break;
	case API::DIRECTX:
		_render = CreateRenderer(API::DIRECTX, _window, PRESENTMODE::IMMEDIATE, *_assetManager);
		break;

	default:
		break;
	}
}

void ModernEngineApplication::InitAssetManager()
{
	_assetManager.reset(new asset::AssetManager());
	_assetManager->Init();
}

void ModernEngineApplication::Draw()
{
	_render->DrawFrame();
}

void ModernEngineApplication::Resize()
{
	switch (_render->getAPI())
	{
	case API::VULKAN:
		static_cast<vulkan::VulkanRenderer*>(_render.get())->_framebufferResized = true;
	default:
		break;
	}
}

void ModernEngineApplication::Finish()
{
	_render->Cleanup();
	_render.reset();
	_assetManager.reset();
}

