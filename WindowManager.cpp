#include "WindowManager.h"
#include "HelloTriangleApplication.h"


WindowManager::WindowManager()
{
}


WindowManager::~WindowManager()
{
}

void WindowManager::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, onWindowResized);
	glfwSetKeyCallback(window, onKeyPressed);
	glfwSetWindowCloseCallback(window, onWindowClose);
}

void WindowManager::shutDown()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void WindowManager::getWindowSize(int* width, int* height)
{
	glfwGetWindowSize(window, width, height);
}

vk::SurfaceKHR WindowManager::createSurface(vk::Instance instance)
{
	VkSurfaceKHR localSurface;
	const VkInstance localInstance = VkInstance(instance);
	if (glfwCreateWindowSurface(localInstance, window, nullptr, &localSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
	return vk::SurfaceKHR(localSurface);
}

bool WindowManager::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void WindowManager::onWindowClose(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void WindowManager::onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void WindowManager::onWindowResized(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;

	HelloTriangleApplication* app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
	app->recreateSwapchain();
}
