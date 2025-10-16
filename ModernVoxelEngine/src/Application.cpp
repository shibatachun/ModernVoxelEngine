#include "Application.h"

Application::Application(GLFWwindow* window) : _window(window)
{
	
}

Application::~Application()
{
	

}

void Application::Init()
{
	InitAssetManager();
	InitRendererBackend(_graphicApi);
	
}

void Application::InitRendererBackend(API api)
{
	switch (api)
	{
	case API::VULKAN:
		_render = CreateRenderer(API::VULKAN, _window, VK_PRESENT_MODE_IMMEDIATE_KHR, *_assetManager);
		
	default:
		break;
	}
}

void Application::InitAssetManager()
{
	_assetManager.reset(new asset::AssetManager());
	_assetManager->Init();
}

void Application::Resize()
{
	switch (_render->getAPI())
	{
	case API::VULKAN:
		static_cast<vulkan::VulkanRenderer*>(_render.get())->_framebufferResized = true;
	default:
		break;
	}
}

void Application::Draw()
{
	_render->DrawFrame();
}

void Application::Finish()
{
	_render->Cleanup();
	_render.reset();
	_assetManager.reset();
}

void Application::SetWindowUserPointer()
{
	glfwSetWindowUserPointer(_window, this);
}


