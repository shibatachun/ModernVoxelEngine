#pragma once
#include <mutex>


#include "Core/GraphicAPI/RenderFactory.h"
#include "ResourcesManager/AssetManager.h"
#include "utils/Camera.h"
#include "Input/InputSystem.h"
#include "Singleton.h"
#include "utils/Window.h"





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
	Application(void* window, API api) : _window(window), _graphicApi(api) {}
	
	API										_graphicApi;
	std::unique_ptr<IRenderer>				_render;
	std::unique_ptr<asset::AssetManager>	_assetManager;
	void* _window;



};

