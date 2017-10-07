//
// Created by Paedda on 04.09.2017.
//


#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <fstream>
//#include <tiny_obj_loader.h>


#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include <unordered_map>


#include "HelloTriangleApplication.h"

using namespace vk;


static VkResult InitDebugReportCallbackEXT(vk::Instance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                           const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(VkInstance(instance),
	                                                                      "vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
	{
		return func(VkInstance(instance), pCreateInfo, pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void UndoDebugReportCallbackEXT(vk::Instance instance, VkDebugReportCallbackEXT callback,
                                       const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
		VkInstance(instance), "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
	{
		func(VkInstance(instance), callback, pAllocator);
	}
}


static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file " + filename);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

void HelloTriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = &debugCallback;

	if (InitDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}

SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(vk::PhysicalDevice device)
{
	SwapChainSupportDetails details;


	//    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

	uint32_t formatCount;
	//    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		//        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	//    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		//        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

vk::SurfaceFormatKHR
HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& avaiableFormats)
{
	if (avaiableFormats.size() == 1 && avaiableFormats[0].format == vk::Format::eUndefined)
	{
		return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
	}

	for (const auto& avaiableFormat : avaiableFormats)
	{
		if (avaiableFormat.format == vk::Format::eB8G8R8A8Unorm &&
			avaiableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return avaiableFormat;
		}
	}

	return avaiableFormats[0];
}

vk::PresentModeKHR
HelloTriangleApplication::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
{
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;// VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& avaiablePresentMode : availablePresentModes)
	{
		if (avaiablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return avaiablePresentMode;
		}
		else if (avaiablePresentMode == vk::PresentModeKHR::eImmediate)
		{
			bestMode = avaiablePresentMode;
		}
	}

	return bestMode;
}

vk::Extent2D HelloTriangleApplication::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		actualExtent.width = std::max(capabilities.minImageExtent.width,
		                              std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
		                               std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void HelloTriangleApplication::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		auto flags = vk::ImageAspectFlagBits::eColor;
		vk::ImageAspectFlags aspectFlags(vk::ImageAspectFlagBits::eColor);
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat,
		                                         vk::ImageAspectFlags(vk::ImageAspectFlagBits::eColor));
	}
}

void HelloTriangleApplication::createSwapChain()
{
	SwapChainSupportDetails swapChainSupportDetails = querySwapChainSupport(physicalDevice);

	vk::SurfaceFormatKHR surfaceFormatKHR = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
	vk::PresentModeKHR presentModeKHR = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
	vk::Extent2D extent2D = chooseSwapExtent(swapChainSupportDetails.capabilities);

	uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
	if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupportDetails.capabilities.maxImageCount)
	{
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;
	}

	//    vk::SwapchainCreateInfoKHR createInfo(
	//            vk::SwapchainCreateFlagBitsKHR ,
	//            surface,);


	vk::SwapchainCreateInfoKHR createInfo;

	//    vk::SwapchainCreateInfoKHR createInfo = {};
	//    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormatKHR.format;
	createInfo.imageColorSpace = surfaceFormatKHR.colorSpace;
	createInfo.imageExtent = extent2D;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;//VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainImageFormat = surfaceFormatKHR.format;
	swapChainExtent = extent2D;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;// VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive; // vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentModeKHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (device.createSwapchainKHR(&createInfo, nullptr, &swapChain) != vk::Result::eSuccess)
	{
		//    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}

	device.getSwapchainImagesKHR(swapChain, &imageCount, nullptr);
	//    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	device.getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages.data());
	//    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}

void HelloTriangleApplication::createSurface()
{
	auto localSurface = VkSurfaceKHR(surface);
	if (glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &localSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
	surface = vk::SurfaceKHR(localSurface);
}

void HelloTriangleApplication::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo;
		//        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	vk::PhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;


	vk::DeviceCreateInfo createInfo;
	//    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (physicalDevice.createDevice(&createInfo, nullptr, &device) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	device.getQueue(indices.graphicsFamily, 0, &graphicsQueue);
	device.getQueue(indices.presentFamily, 0, &presentQueue);
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;

	unsigned int queueFamilyCount = 0;
	device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		vk::Bool32 presentSupport = false;
		device.getSurfaceSupportKHR(i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}
		i++;
	}

	return indices;
}

bool HelloTriangleApplication::isDeviceSuitable(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainSupported = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainSupported = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	vk::PhysicalDeviceFeatures supportedFeatures;
	device.getFeatures(&supportedFeatures);
	//    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() &&
		extensionsSupported &&
		swapChainSupported &&
		supportedFeatures.samplerAnisotropy;
}

bool HelloTriangleApplication::checkDeviceExtensionSupport(vk::PhysicalDevice device)
{
	uint32_t extensionCount;
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);
	//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
	//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void HelloTriangleApplication::pickPhysicalDevice()
{
	unsigned int deviceCount = 0;
	instance.enumeratePhysicalDevices(&deviceCount, nullptr);
	//    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}

	std::vector<vk::PhysicalDevice> devices(deviceCount);
	instance.enumeratePhysicalDevices(&deviceCount, devices.data());
	//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU");
	}
}

void HelloTriangleApplication::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not avaiable");
	}

	vk::ApplicationInfo appInfo = {};
	//    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "RostyEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	vk::InstanceCreateInfo createInfo = {};
	//    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	checkRequiredExtensionsAreSupported(glfwExtensions);

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();


	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	auto result = vk::createInstance(&createInfo, nullptr, &instance);
	//    vk::Result result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create vulkan instance");
	}
	else
	{
		std::cout << "Successfully created Vulkan instance" << std::endl;
	}
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
	unsigned int layerCount;

	vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<vk::LayerProperties> avaiableLayers(layerCount);
	vk::enumerateInstanceLayerProperties(&layerCount, avaiableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : avaiableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char *> HelloTriangleApplication::getRequiredExtensions()
{
	std::vector<const char *> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (unsigned int i = 0; i < glfwExtensionCount; i++)
	{
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	return extensions;
}

void HelloTriangleApplication::checkRequiredExtensionsAreSupported(const char** pString)
{
	unsigned int extensionCount = 0;
	vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<vk::ExtensionProperties> extensions(extensionCount);
	vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	for (const auto& extension : extensions)
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}
}

void HelloTriangleApplication::cleanup()
{
	device.destroySemaphore(renderFinishedSemaphore, nullptr);
	device.destroySemaphore(imageAvailableSemaphore, nullptr);

	cleanupSwapChain();

	device.destroySampler(textureSampler);
	device.destroyImageView(textureImageView);
	device.destroyImage(textureImage);
	device.freeMemory(textureImageMemory);

	device.destroyDescriptorPool(descriptorPool);
	device.destroyDescriptorSetLayout(descriptorSetLayout);

	device.destroyDescriptorPool(descriptorPool);
	device.destroyDescriptorSetLayout(descriptorSetLayout);

	device.destroyBuffer(uniformBuffer);
	device.freeMemory(uniformBufferMemory);

	device.destroyBuffer(indexBuffer);
	device.freeMemory(indexBufferMemory);

	device.destroyBuffer(vertexBuffer);
	device.freeMemory(vertexBufferMemory);

	device.destroyCommandPool(commandPool);

	device.destroy();

	//instance.destroyDebugReportCallbackEXT(callback, nullptr);
	UndoDebugReportCallbackEXT(instance, callback, nullptr);

	instance.destroySurfaceKHR(surface, nullptr);
	instance.destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		updateUniformBuffer();
		drawFrame();
	}
	device.waitIdle();
}

using namespace vk;

void HelloTriangleApplication::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo,
		fragShaderStageInfo
	};

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());

	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::Viewport viewport;
	viewport.x, viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vk::Rect2D scissor;
	scissor.offset = vk::Offset2D{0, 0};
	scissor.extent = swapChainExtent;

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;

	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	vk::PipelineColorBlendAttachmentState colorBlendAttachment;

	vk::ColorComponentFlags flag(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB);
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;

#ifdef RH_COLOR_BLENDING
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
#else
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional
#endif

	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = vk::StencilOpState{};
	depthStencil.back = vk::StencilOpState{};

	vk::DynamicState dynamicStates[] = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eLineWidth
	};

	vk::PipelineDynamicStateCreateInfo dynamicState;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	if (device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}


	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = &depthStencil;

	pipelineInfo.layout = pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	graphicsPipeline = device.createGraphicsPipeline(NULL, pipelineInfo, nullptr);

	device.destroyShaderModule(fragShaderModule);
	device.destroyShaderModule(vertShaderModule);
}

vk::ShaderModule HelloTriangleApplication::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
	vk::ShaderModule shaderModule;
	if (device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create shader module");
	}

	return shaderModule;
}

void HelloTriangleApplication::createRenderPass()
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;

	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
	depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::AttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

	vk::RenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassCreateInfo.pAttachments = attachments.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	vk::SubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	//dependency.srcAccessMask = vk::AccessFlagBits:: 0;
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	renderPass = device.createRenderPass(renderPassCreateInfo, nullptr);
}

void HelloTriangleApplication::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		std::array<vk::ImageView, 2> attachments = {
			swapChainImageViews[i],
			depthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo, nullptr);
	}
}

void HelloTriangleApplication::createCommandPool()
{
	const QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	vk::CommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	commandPool = device.createCommandPool(poolCreateInfo, nullptr);
}

void HelloTriangleApplication::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	commandBuffers = device.allocateCommandBuffers(allocInfo);
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		commandBuffers[i].begin(beginInfo);

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<vk::ClearValue, 2> clearValues = {};
		clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = {vertexBuffer};
		vk::DeviceSize offsets[] = {0};

		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0,
		                                     nullptr);
		commandBuffers[i].drawIndexed(model.indices.size(), 1, 0, 0, 0);
		commandBuffers[i].endRenderPass();

		commandBuffers[i].end();
	}
}

void HelloTriangleApplication::drawFrame()
{
	vk::ResultValue<uint32_t> imageIndex = device.acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(),
	                                                                  imageAvailableSemaphore, VK_NULL_HANDLE);


	if (imageIndex.result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapchain();
		return;
	}
	else if (imageIndex.result != vk::Result::eSuccess && imageIndex.result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = {imageAvailableSemaphore};
	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex.value];

	vk::Semaphore signaleSemaphores[] = {renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signaleSemaphores;

	graphicsQueue.submit(submitInfo, VK_NULL_HANDLE);

	vk::PresentInfoKHR presentInfoKHR = {};

	presentInfoKHR.waitSemaphoreCount = 1;
	presentInfoKHR.pWaitSemaphores = signaleSemaphores;

	vk::SwapchainKHR swapchainKHR[] = {swapChain};
	presentInfoKHR.swapchainCount = 1;
	presentInfoKHR.pSwapchains = swapchainKHR;
	presentInfoKHR.pImageIndices = &imageIndex.value;

	presentInfoKHR.pResults = nullptr;

	vk::Result result = presentQueue.presentKHR(presentInfoKHR);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		recreateSwapchain();
		return;
	}
	presentQueue.waitIdle();
}

void HelloTriangleApplication::createSemaphores()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	vk::Result imgAvResult = device.createSemaphore(&semaphoreCreateInfo, nullptr, &imageAvailableSemaphore);
	vk::Result rndFnResult = device.createSemaphore(&semaphoreCreateInfo, nullptr, &renderFinishedSemaphore);
}

void HelloTriangleApplication::recreateSwapchain()
{
	device.waitIdle();

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void HelloTriangleApplication::cleanupSwapChain()
{
	device.destroyImageView(depthImageView);
	device.destroyImage(depthImage);
	device.freeMemory(depthImageMemory);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
	{
		device.destroyFramebuffer(swapChainFramebuffers[i], nullptr);
	}

	device.freeCommandBuffers(commandPool, commandBuffers.size(), commandBuffers.data());

	device.destroyPipeline(graphicsPipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderPass);
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		device.destroyImageView(swapChainImageViews[i]);
	}

	device.destroySwapchainKHR(swapChain);
}

void HelloTriangleApplication::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(model.vertices[0]) * model.vertices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;


	createBuffer(bufferSize,
	             vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	             stagingBuffer,
	             stagingBufferMemory);


	void* data;
	device.mapMemory(stagingBufferMemory, 0,bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, model.vertices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize,
	             vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
	             vk::MemoryPropertyFlagBits::eDeviceLocal,
	             vertexBuffer,
	             vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

uint32_t HelloTriangleApplication::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

void HelloTriangleApplication::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
                                            vk::MemoryPropertyFlags memoryPropertyFlags, vk::Buffer& buffer,
                                            vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	device.createBuffer(&bufferCreateInfo, nullptr, &buffer);

	const vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocateInfo = {};
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	device.allocateMemory(&allocateInfo, nullptr, &bufferMemory);
	device.bindBufferMemory(buffer, bufferMemory, 0);
}


void HelloTriangleApplication::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferCopy copyRegion = {};
	copyRegion.size = size;
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(model.indices[0]) * model.indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer,
	             stagingBufferMemory);

	void* data;
	device.mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, model.indices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
	             vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void HelloTriangleApplication::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::DescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

	vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();


	descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
}

void HelloTriangleApplication::createUniformBuffer()
{
	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
	createBuffer(bufferSize,
	             vk::BufferUsageFlagBits::eUniformBuffer,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	             uniformBuffer,
	             uniformBufferMemory);
}

void HelloTriangleApplication::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f,
	                            10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	device.mapMemory(uniformBufferMemory, 0, sizeof(ubo), vk::MemoryMapFlags(), &data);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformBufferMemory);
}

void HelloTriangleApplication::loadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH.c_str()))
	{
		throw std::runtime_error(err);
	}

	model = {};
	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = {1.0f, 1.0f, 1.0f};

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
				model.vertices.push_back(vertex);
			}

			model.indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void HelloTriangleApplication::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = vk::DescriptorType::eUniformBuffer;;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	poolSizes[1].descriptorCount = 1;


	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;
	descriptorPool = device.createDescriptorPool(poolInfo);
}

void HelloTriangleApplication::createDescriptorSet()
{
	vk::DescriptorSetLayout layouts[] = {descriptorSetLayout};
	vk::DescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.descriptorPool = descriptorPool;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = layouts;

	device.allocateDescriptorSets(&allocateInfo, &descriptorSet);

	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	vk::DescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageInfo.imageView = textureImageView;
	imageInfo.sampler = textureSampler;

	std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void HelloTriangleApplication::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texHeight * texWidth * 4;
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture");
	}

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	createBuffer(imageSize,
	             vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	             stagingBuffer,
	             stagingBufferMemory);
	void* data;
	device.mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlags(), &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	device.unmapMemory(stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(static_cast<uint32_t>(texWidth),
	            static_cast<uint32_t>(texHeight),
	            vk::Format::eR8G8B8A8Unorm,
	            vk::ImageTiling::eLinear,
	            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
	            vk::MemoryPropertyFlagBits::eDeviceLocal,
	            textureImage,
	            textureImageMemory);

	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined,
	                      vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal,
	                      vk::ImageLayout::eShaderReadOnlyOptimal);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void
HelloTriangleApplication::createImage(uint32_t texWidth, uint32_t texHeight, vk::Format format, vk::ImageTiling tilig,
                                      vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties,
                                      vk::Image& image, vk::DeviceMemory& imageMemory)
{
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = texWidth;
	imageInfo.extent.height = texHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tilig;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = usage;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	//    imageInfo.flags = 0;


	image = device.createImage(imageInfo);

	vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocateInfo = {};
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits,  memoryProperties);
	device.allocateMemory(&allocateInfo, nullptr, &imageMemory);
	device.bindImageMemory(image, imageMemory, 0);
}

vk::CommandBuffer HelloTriangleApplication::beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.level = vk::CommandBufferLevel::ePrimary;
	allocateInfo.commandPool = commandPool;
	allocateInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer;

	device.allocateCommandBuffers(&allocateInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(&beginInfo);
	return commandBuffer;
}


void HelloTriangleApplication::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	graphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void HelloTriangleApplication::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                                                     vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

		if (hasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::
			eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage,
	                              vk::DependencyFlags(),
	                              0, nullptr,
	                              0, nullptr,
	                              1, &barrier);
	/*vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);*/

	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = vk::Offset3D{0, 0, 0};
	region.imageExtent = vk::Extent3D{
		width, height, 1
	};


	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::createTextureImageView()
{
	textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
}

vk::ImageView
HelloTriangleApplication::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo;
	//    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = vk::ImageViewType::e2D; // VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	vk::ImageView imageView = device.createImageView(viewInfo);

	return imageView;
}

void HelloTriangleApplication::createTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo = {};
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;

	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = vk::CompareOp::eAlways;

	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	textureSampler = device.createSampler(samplerInfo);
}

void HelloTriangleApplication::createDepthResources()
{
	vk::Format depthFormat = findDepthFormat();
	createImage(
		swapChainExtent.width,
		swapChainExtent.height,
		depthFormat,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		depthImage,
		depthImageMemory
	);
	depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
	transitionImageLayout(
		depthImage,
		depthFormat,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal
	);
}

vk::Format
HelloTriangleApplication::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
                                              vk::FormatFeatureFlags features)
{
	for (vk::Format format : candidates)
	{
		vk::FormatProperties props = physicalDevice.getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	throw std::runtime_error("Failed to find supported format");
}
