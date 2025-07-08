#include "utils/GlfwHelper.h"
#include "Application.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> 


std::unique_ptr<asset::AssetManager> g_assetManager;


static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<IRenderer*>(glfwGetWindowUserPointer(window));
	//app->framebufferResized = true;
}
int main() {
	uint32_t width = 1280;
	uint32_t height = 800;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	
	g_assetManager.reset(new asset::AssetManager());
	
	if (!g_assetManager->Init())
	{
		std::cerr << "Failed to initialize assetManager" << std::endl;
		return -1;
	}
	auto renderer = CreateRenderer(api, window, VK_PRESENT_MODE_FIFO_RELAXED_KHR, *g_assetManager);
	auto _shaderAssets = g_assetManager->getShaderAssets();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		renderer->DrawFrame();
	}
	renderer->Cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
