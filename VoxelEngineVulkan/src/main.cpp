#include "Application.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	app->Resize();
}
int main() {
	uint32_t width = 1280;
	uint32_t height = 800;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	auto& app = Application::getInstance(window);
	app.Init();
	app.SetWindowUserPointer();
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		app.Draw();
	}
	app.Finish();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
