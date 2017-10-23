#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

class WindowManager
{
private:
	GLFWwindow* window;
public:
	WindowManager();
	~WindowManager();

	void init();
	void shutDown();
	void getWindowSize(int* width, int* height);
	vk::SurfaceKHR createSurface(vk::Instance instance);
	bool shouldClose();

	static void onWindowClose(GLFWwindow* window);
	static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onWindowResized(GLFWwindow* window, int width, int height);
};
