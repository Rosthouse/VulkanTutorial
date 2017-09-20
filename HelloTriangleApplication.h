//
// Created by Paedda on 04.09.2017.
//

#ifndef VULKAN_HELLOTRIANGLEAPPLICATION_H
#define VULKAN_HELLOTRIANGLEAPPLICATION_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <set>
#include <limits>
#include <chrono>

#include <VulkanTutorialConfig.h>
#include "Vertex.h"

const int WIDTH = 800;
const int HEIGHT = 600;

static VkResult CreateDebugReportCallbackEXT(VkInstance instance,
                                             const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugReportCallbackEXT *pCallback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                          const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}


struct Model {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    const VkIndexType indexType = VK_INDEX_TYPE_UINT16;

    const size_t verticesCount() {
        return static_cast<size_t>(vertices.size());
    }

    const size_t indicesCount() {
        return static_cast<size_t>(indices.size());
    }
};

const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
};

//const Model model = {
//        {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//                      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}},
//        {0,     1, 2, 2, 3, 0}
//};

class HelloTriangleApplication {
public:

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t obj,
            size_t location,
            int32_t code,
            const char *layerPrefix,
            const char *msg,
            void *userData) {
        std::cerr << "validation layer: " << msg << std::endl;

        return VK_FALSE;
    }


private:
    GLFWwindow *window;
    VkInstance instance;
    VkDebugReportCallbackEXT callback;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;


    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, HelloTriangleApplication::onWindowResized);
        glfwSetKeyCallback(window, HelloTriangleApplication::onKeyPressed);
    }


    void createRenderPass();

    void createFramebuffers();

    void createCommandPool();

    void createCommandBuffers();

    void createSemaphores();

    void createVertexBuffer();

    void createIndexBuffer();

    void createDescriptorSetLayout();

    void createUniformBuffer();

    void createDescriptorPool();

    void createDescriptorSet();

    void createTextureImage();

    void initVulkan() {
        createInstance();
        setupDebugCallback();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createTextureImage();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffer();
        createDescriptorPool();
        createDescriptorSet();
        createCommandBuffers();
        createSemaphores();
    }

    void recreateSwapchain();

    void mainLoop();

    void cleanup();

    void createInstance();

    void createLogicalDevice();

    void createSurface();

    void createSwapChain();

    void createImageViews();

    void createGraphicsPipeline();

    void checkRequiredExtensionsAreSupported(const char **pString);

    std::vector<const char *> getRequiredExtensions();

    bool checkValidationLayerSupport();

    void setupDebugCallback();

    void pickPhysicalDevice();

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avaiableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkShaderModule createShaderModule(const std::vector<char> &code);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void drawFrame();

    void cleanupSwapChain();

    static void onWindowResized(GLFWwindow *window, int width, int height) {
        if (width == 0 || height == 0) return;

        HelloTriangleApplication *app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
        app->recreateSwapchain();
    }

    static void  onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods){
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);

    /**
     * Copies data from a source buffer to a destination buffer.
     *
     * @param srcBuffer
     * @param dstBuffer
     * @param size
     */
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void updateUniformBuffer();

    void createImage(uint32_t texWidth, uint32_t texHeight, VkFormat format, VkImageTiling tilig, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags memoryProperties, VkImage &image, VkDeviceMemory &imageMemory);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};


#endif //VULKAN_HELLOTRIANGLEAPPLICATION_H
