#include "Window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
static SDL_Window* window = nullptr;

static float sdl_get_monitor_refresh() {
	SDL_DisplayMode current;
	int should_be_zero = SDL_GetCurrentDisplayMode(0, &current);
	return 1.0f / current.refresh_rate;
}
void Window::Init(void* configuration_)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return;
	}
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);

	WindowConfiguration& configuration = *(WindowConfiguration*)configuration_;

	SDL_WindowFlags window_flags = (SDL_WindowFlags)0;
	switch (configuration.api)
	{
	case API::VULKAN:
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		break;
	case API::DIRECTX:
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		break;
	case API::OPENGL:
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	default:
		window_flags = SDL_WINDOW_RESIZABLE;
		break;
	}
	window = SDL_CreateWindow(configuration.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, configuration.width, configuration.height, window_flags);
	if (!window) {
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
	}
	platform_handle = window;
	int window_width, window_height;
	switch (configuration.api)
	{
	case API::VULKAN:
		SDL_Vulkan_GetDrawableSize(window, &window_width, &window_height);
		break;

	case API::DIRECTX:
		// DX12 推荐使用逻辑像素大小
		SDL_GetWindowSize(window, &window_width, &window_height);
		break;

	case API::OPENGL:
		// 与 Vulkan 类似，也需要 drawable size
		SDL_GL_GetDrawableSize(window, &window_width, &window_height);
		break;
	}

	// 转成你的 u32
	width = (uint32_t)window_width;
	height = (uint32_t)window_height;
	dispaly_refresh = sdl_get_monitor_refresh();
}
void Window::Shutdown() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}
void Window::HandleOSMessages()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
		{
			requested_exit = true;
		}
		default:
			break;
		}
	}
}

void Window::SetFullScreen(bool value)
{
}

void Window::RegisterOSMessagesCallback(OsMessagesCallBack callback, void* user_data_)
{
}

void Window::UnRegisterOsMessagesCallback(OsMessagesCallBack callback)
{
}

void Window::CenterMouse(bool dragging)
{
}
