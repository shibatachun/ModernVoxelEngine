#pragma once
#include <mutex>


#include "Core/GraphicAPI/RenderFactory.h"
#include "ResourcesManager/AssetManager.h"
#include "utils/GLFWHelper.h"
#include "utils/Camera.h"
#include "Singleton.h"



#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> 
//class Application
//{
//public:
//	Application(GLFWwindow* window);
//	~Application();
//	Application(const Application&) = delete;
//	Application& operator=(const Application&) = delete;
//
//	static std::unique_ptr<Application> instance;
//	static std::once_flag initFlag;
//public:
//	static Application& getInstance(GLFWwindow* window) {
//		std::call_once(initFlag, [&]() {
//			instance.reset(new Application(window));
//			});
//		return *instance;
//	}
//	void Init();
//	void InitRendererBackend(API api);
//	void InitAssetManager();
//	void Resize();
//	void Draw();
//	void Finish();
//	void SetWindowUserPointer();
//
//public:
//	bool _frameBufferResize = false;
//private:
//	API										_graphicApi;
//	std::unique_ptr<IRenderer>				_render;
//	std::unique_ptr<asset::AssetManager>	_assetManager;
//	GLFWwindow*								_window;
//
//};
//inline std::unique_ptr<Application>		Application::instance;
//inline std::once_flag					Application::initFlag;

class Application
{
public:
	virtual ~Application() = default;
	virtual void Init() = 0;
	virtual void InitRendererBackend(API api) = 0;
	virtual void InitAssetManager() = 0;
	virtual void Draw() = 0;
	virtual void Resize() = 0;
	virtual void Finish() = 0; 
	virtual void SetWindowUserPointer();
protected:
	Application(GLFWwindow* window, API api) : _window(window), _graphicApi(api) {}
	
	API										_graphicApi;
	std::unique_ptr<IRenderer>				_render;
	std::unique_ptr<asset::AssetManager>	_assetManager;
	GLFWwindow* _window;



};

