#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <cstdlib>
#include "HelloTriangleApplication.h"

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}