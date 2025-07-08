#include "Application.h"

Application::~Application()
{
	_render.reset();

}

void Application::Init()
{
	InitRendererBackend(_graphicApi);
	InitAssetManager();
}

void Application::InitRendererBackend(API api)
{

}

void Application::InitAssetManager()
{
}
