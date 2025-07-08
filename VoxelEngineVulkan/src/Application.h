#pragma once
#include <mutex>
#include <memory>

#include "GraphicAPI/RenderFactory.h"
#include "ResourcesManager/AssetManager.h"
#include "utils/GlfwHelper.h"
class Application
{
	Application(GLFWwindow* window);
	~Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	static std::unique_ptr<Application> instance;
	static std::once_flag initFlag;
public:
	static Application& getInstance(GLFWwindow* window) {
		std::call_once(initFlag, [&]() {
			instance.reset(new Application(window));
			});
		return *instance;
	}
	void Init();
	void InitRendererBackend(API api);
	void InitAssetManager();
	
public:
	const bool _frameBufferResize = false;
private:
	API										_graphicApi;
	std::unique_ptr<IRenderer>				_render;
	std::unique_ptr<asset::AssetManager>	_assetManager;
	GLFWwindow*								_window;

};
std::unique_ptr<Application>	Application::instance;
std::once_flag					Application::initFlag;

