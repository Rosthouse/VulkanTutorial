//
// Created by Paedda on 04.09.2017.
//

#ifndef VULKAN_HELLOTRIANGLEAPPLICATION_H
#define VULKAN_HELLOTRIANGLEAPPLICATION_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

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

const std::string MODEL_PATH = "models/chalet.obj";
const std::string TEXTURE_PATH = "textures/chalet.jpg";


//static vk::Result CreateDebugReportCallbackEXT(vk::Instance instance,
//                                             const vk::DebugReportCallbackCreateInfoEXT *pCreateInfo,
//                                             const vk::AllocationCallbacks *pAllocator,
//                                             vk::DebugReportCallbackEXT *pCallback) {
//
//    instance.createDebugReportCallbackEXT(pCreateInfo, pAllocator, pCallback);
//    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
//    if (func != nullptr) {
//        return func(instance, pCreateInfo, pAllocator, pCallback);
//    } else {
//        return VK_ERROR_EXTENSION_NOT_PRESENT;
//    }
//}

//static void DestroyDebugReportCallbackEXT(vk::Instance instance, vk::DebugReportCallbackEXT callback,
//                                          const vk::AllocationCallbacks *pAllocator) {
//    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance,
//                                                                            "vkDestroyDebugReportCallbackEXT");
//    if (func != nullptr) {
//        func(instance, callback, pAllocator);
//    }
//}

class HelloTriangleApplication {
public:

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugReportFlagsEXT flags,
            vk::DebugReportObjectTypeEXT objType,
            uint64_t obj,
            size_t location,
            int32_t code,
            const char *layerPrefix,
            const char *msg,
            void *userData) {
        std::cerr << "validation layer: " << msg << std::endl;

        return false;
//        return VK_FALSE;
    }


private:
    GLFWwindow *window;
    vk::Instance instance;
    vk::DebugReportCallbackEXT callback;
    vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;
    vk::SwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;
    std::vector<vk::ImageView> swapChainImageViews;
    vk::RenderPass renderPass;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;
    vk::Buffer uniformBuffer;
    vk::DeviceMemory uniformBufferMemory;
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSet descriptorSet;
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;
    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;
    Model model;


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

    void createTextureImageView();

    void createTextureSampler();

    void createDepthResources();

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
        createCommandPool();
        createDepthResources();
        createFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        loadModel();
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

    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

    bool isDeviceSuitable(vk::PhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &avaiableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);


    vk::ShaderModule createShaderModule(const std::vector<char> &code);

    vk::CommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    void drawFrame();

    void cleanupSwapChain();

    static void onWindowResized(GLFWwindow *window, int width, int height) {
        if (width == 0 || height == 0) return;

        HelloTriangleApplication *app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
        app->recreateSwapchain();
    }

    static void onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
                      vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);

    /**
     * Copies data from a source buffer to a destination buffer.
     *
     * @param srcBuffer
     * @param dstBuffer
     * @param size
     */
    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    void updateUniformBuffer();

    void loadModel();

    void
    createImage(uint32_t texWidth, uint32_t texHeight, vk::Format format, vk::ImageTiling tilig,
                vk::ImageUsageFlags usage,
                vk::MemoryPropertyFlags memoryProperties, vk::Image &image, vk::DeviceMemory &imageMemory);

    void
    transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    vk::Image createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

    vk::Format
    findSupportedFormat(const std::vector<vk::Format> &pVector, vk::ImageTiling tiling,
                        vk::FormatFeatureFlags features);


    vk::Format findDepthFormat() {
        return findSupportedFormat({
                                           vk::Format::eD32Sfloat,
                                           vk::Format::eD32SfloatS8Uint,
                                           vk::Format::eD24UnormS8Uint,
                                   },
                                   vk::ImageTiling::eOptimal,
                                   vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    bool hasStencilComponent(vk::Format format) {
        return format == vk::Format::eD32Sfloat || format == vk::Format::eD32SfloatS8Uint;
    }
};


#endif //VULKAN_HELLOTRIANGLEAPPLICATION_H
