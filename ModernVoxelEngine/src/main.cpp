#include "ModernEngine/ModernEngineApplication.h"
#include "VoxelEngine/VoxelApplication.h"

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

struct MouseState {
	glm::vec2 position{ 0.0f };
	struct {
		bool left{ false };
		bool right{ false };
		bool middle{ false };
	} buttons;
} mouseState;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mouseState.position = glm::vec2((float)xpos, (float)ypos);

	bool pressed = (action == GLFW_PRESS);
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseState.buttons.left = pressed;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseState.buttons.right = pressed;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mouseState.buttons.middle = pressed;
		break;
	}
}
bool viewUpdated = false;
bool paused = false;
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
	if (mouseState.buttons.left) {
		glm::vec2 delta = mouseState.position - glm::vec2((float)xpos, (float)ypos);
		gCamera.rotate(glm::vec3(delta.y, -delta.x, 0.0f));
		
	}
	mouseState.position = glm::vec2((float)xpos, (float)ypos);
}
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	gCamera.translate(glm::vec3(0.0f, 0.0f, (float)yoffset * 0.5f));
	viewUpdated = true;
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		printf("Pressed key: %d (scancode=%d)\n", key, scancode);
		switch (key) {
		case GLFW_KEY_F2:
			if (gCamera.type == Camera::CameraType::lookat)
				gCamera.type = Camera::CameraType::firstperson;
			else
				gCamera.type = Camera::CameraType::lookat;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}

		if (gCamera.type == Camera::firstperson) {
			switch (key) {
			case GLFW_KEY_W: gCamera.keys.up = true; break;
			case GLFW_KEY_S: gCamera.keys.down = true; break;
			case GLFW_KEY_A: gCamera.keys.left = true; break;
			case GLFW_KEY_D: gCamera.keys.right = true; break;
			}
		}

	
	}

	else if (action == GLFW_RELEASE) {
		if (gCamera.type == Camera::firstperson) {
			switch (key) {
			case GLFW_KEY_W: gCamera.keys.up = false; break;
			case GLFW_KEY_S: gCamera.keys.down = false; break;
			case GLFW_KEY_A: gCamera.keys.left = false; break;
			case GLFW_KEY_D: gCamera.keys.right = false; break;
			}
		}
	}
}

int main() {
	uint32_t width = 1920;
	uint32_t height = 1080;
	GLFWwindow* window = initWindow("GLFW example", width, height);
	API api = API::VULKAN;
	//auto& app = Application::getInstance(window);
	auto& app = ModernEngineApplication::GetInstance(window);
	app.Init();
	app.SetWindowUserPointer();
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	
	gCamera.type = Camera::CameraType::firstperson;
	gCamera.flipY = false;
	gCamera.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	gCamera.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
	gCamera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 256.0f);
	float frameCounter = 0.0f;
	float frameTimer = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		

		auto tStart = std::chrono::high_resolution_clock::now();
		app.Draw();
		frameCounter++;
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		frameTimer = (float)tDiff / 1000.0f;
		gCamera.update(frameTimer);

		TitleFps("lihai", window);
	}
	app.Finish();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

