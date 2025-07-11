#include "Application.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	app->Resize();
}

void TitleFps(std::string windowTitle, GLFWwindow* pWindow)
{
	static double time0 = glfwGetTime();
	static double  time1;
	static double dt;
	static int dframe = -1;
	static std::stringstream info;
	time1 = glfwGetTime();
	dframe++;
	if ((dt = time1 - time0) >= 1)
	{
		info.precision(1);
		info << windowTitle << " " << std::fixed << dframe / dt << " FPS";
		glfwSetWindowTitle(pWindow, info.str().c_str());
		info.str("");
		time0 = time1;
		dframe = 0;
	}

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
		

		TitleFps("lihai", window);
	}
	app.Finish();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

