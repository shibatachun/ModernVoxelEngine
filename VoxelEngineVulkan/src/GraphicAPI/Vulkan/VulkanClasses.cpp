#include "VulkanClasses.h"

vulkan::Instance::Instance(GLFWwindow* window) : _window(window)
{
	
	CheckVulkanMinimumVersion(_vulkanVersion);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	auto extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else 
	const bool enableValidationLayers = true;
#endif
	if (enableValidationLayers)
	{
		validationLayers_.push_back("VK_LAYER_KHRONOS_validation");
	}
	CheckValidationLayerSupport(validationLayers_);
	if (!validationLayers_.empty())
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Homemade_Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VulkanEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = _vulkanVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers_.size());
	createInfo.ppEnabledLayerNames = validationLayers_.data();

	Check(vkCreateInstance(&createInfo, nullptr, &instance_), "create instance");
	GetVulkanPhysicalDevices();

}

vulkan::Instance::~Instance()
{
	if (instance_ != nullptr)
	{
		vkDestroyInstance(instance_, nullptr);
		instance_ = nullptr;
	}
}

GLFWwindow* vulkan::Instance::getWindow() const
{
	return _window;
}

void vulkan::Instance::GetVulkanExtensions()
{

}

void vulkan::Instance::GetVulkanLayers()
{
}

void vulkan::Instance::GetVulkanPhysicalDevices()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
	deviceCount == 0 ? throw std::runtime_error("failed to find GPUs with Vulkan support!")  : physicalDevices_.resize(deviceCount);
	vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices_.data());

}

void vulkan::Instance::CheckVulkanMinimumVersion(const uint32_t minVersion)
{
	uint32_t version;
	Check(vkEnumerateInstanceVersion(&version), "query instance version");
	if (minVersion > version)
	{
		
		throw std::runtime_error("minimum required version not found");
	}
}

void vulkan::Instance::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{


	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	for (const char* layerName : validationLayers)
	{
#ifdef _DEBUG
		std::cout << layerName << std::endl;
#endif // DEBUG
		bool layerFound = false;
		for (const auto& LayerProperties : availableLayers)
		{
#ifdef _DEBUG
			std::cout << LayerProperties.layerName << std::endl;
#endif // DEBUG
			if (strcmp(layerName, LayerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			throw std::runtime_error("cound not find the requested validation layer");
		}
	}
}

vulkan::Surface::Surface(const Instance& instance) : instance_(instance)
{
	instance_.getWindow();
	Check(glfwCreateWindowSurface(instance_.Handle(), instance_.getWindow(), nullptr, &surface_), "create window surface");
}

vulkan::Surface::~Surface()
{
	if (surface_ != nullptr)
	{
		vkDestroySurfaceKHR(instance_.Handle(), surface_, nullptr);
	}
}

namespace
{

	const char* ObjectTypeToString(const VkObjectType objectType)
	{
		switch (objectType)
		{
#define STR(e) case VK_OBJECT_TYPE_ ## e: return # e
			STR(UNKNOWN);
			STR(INSTANCE);
			STR(PHYSICAL_DEVICE);
			STR(DEVICE);
			STR(QUEUE);
			STR(SEMAPHORE);
			STR(COMMAND_BUFFER);
			STR(FENCE);
			STR(DEVICE_MEMORY);
			STR(BUFFER);
			STR(IMAGE);
			STR(EVENT);
			STR(QUERY_POOL);
			STR(BUFFER_VIEW);
			STR(IMAGE_VIEW);
			STR(SHADER_MODULE);
			STR(PIPELINE_CACHE);
			STR(PIPELINE_LAYOUT);
			STR(RENDER_PASS);
			STR(PIPELINE);
			STR(DESCRIPTOR_SET_LAYOUT);
			STR(SAMPLER);
			STR(DESCRIPTOR_POOL);
			STR(DESCRIPTOR_SET);
			STR(FRAMEBUFFER);
			STR(COMMAND_POOL);
			STR(SAMPLER_YCBCR_CONVERSION);
			STR(DESCRIPTOR_UPDATE_TEMPLATE);
			STR(SURFACE_KHR);
			STR(SWAPCHAIN_KHR);
			STR(DISPLAY_KHR);
			STR(DISPLAY_MODE_KHR);
			STR(DEBUG_REPORT_CALLBACK_EXT);
			STR(DEBUG_UTILS_MESSENGER_EXT);
			STR(ACCELERATION_STRUCTURE_KHR);
			STR(VALIDATION_CACHE_EXT);
			STR(PERFORMANCE_CONFIGURATION_INTEL);
			STR(DEFERRED_OPERATION_KHR);
			STR(INDIRECT_COMMANDS_LAYOUT_NV);
#undef STR
		default: return "unknown";
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
		void* const pUserData)
	{
		(void)pUserData;

		

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			std::cerr << "VERBOSE: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			std::cerr << "INFO: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cerr << "WARNING: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cerr << "ERROR: ";
			break;
		default:;
			std::cerr << "UNKNOWN: ";
		}

		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			std::cerr << "GENERAL: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			std::cerr << "VALIDATION: ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			std::cerr << "PERFORMANCE: ";
			break;
		default:
			std::cerr << "UNKNOWN: ";
		}

		std::cerr << pCallbackData->pMessage;

		if (pCallbackData->objectCount > 0 && messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			std::cerr << "\n\n  Objects (" << pCallbackData->objectCount << "):\n";

			for (uint32_t i = 0; i != pCallbackData->objectCount; ++i)
			{
				const auto object = pCallbackData->pObjects[i];
				std::cerr
					<< "  - Object[" << i << "]: "
					<< "Type: " << ObjectTypeToString(object.objectType) << ", "
					<< "Handle: " << reinterpret_cast<void*>(object.objectHandle) << ", "
					<< "Name: '" << (object.pObjectName ? object.pObjectName : "") << "'"
					<< "\n";
			}
		}

		std::cerr << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
	{
		const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		return func != nullptr
			? func(instance, pCreateInfo, pAllocator, pCallback)
			: VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
	{
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}
}

vulkan::DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold)
	: instance_(instance), threshold_(threshold)
{
	if (instance.GetValidationLayers().empty())
	{
		return;
	}

	VkDebugUtilsMessageSeverityFlagsEXT severity = 0;


	switch (threshold)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		break;
	default:
		throw (std::invalid_argument("invalid threshold"));
	}


	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = severity;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = nullptr;

	Check(CreateDebugUtilsMessengerEXT(instance_.Handle(), &createInfo, nullptr, &messenger_),
		"set up Vulkan debug callback");


}

vulkan::DebugUtilsMessenger::~DebugUtilsMessenger()
{
	if (messenger_ != nullptr)
	{
		DestroyDebugUtilsMessengerEXT(instance_.Handle(), messenger_, nullptr);
		messenger_ = nullptr;
	}
}

vulkan::DebugUtils::DebugUtils(VkInstance instance) :
	vkSetDebugUtilsObjectNameEXT_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")))
{
#ifndef NDEBUG
	if (vkSetDebugUtilsObjectNameEXT_ == nullptr)
	{
		throw (std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'"));
	}
#endif
}

namespace
{
	std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
		const std::vector<VkQueueFamilyProperties>& queueFamilies,
		const std::string& name,
		const VkQueueFlags requiredBits,
		const VkQueueFlags excludedBits)
	{
		const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
			{
				return
					queueFamily.queueCount > 0 &&
					queueFamily.queueFlags & requiredBits &&
					!(queueFamily.queueFlags & excludedBits);
			});

		if (family == queueFamilies.end())
		{
			throw (std::runtime_error("found no matching " + name + " queue"));
		}

		return family;
	}
}
vulkan::Device::Device(
	VkPhysicalDevice device, 
	const Surface& surface, 
	const std::vector<const char*>& requiredExtensions, 
	const void* nextDeviceFeatures):
	_physicalDevice(device),
	_surface(surface),
	_debugUtils(surface.getInstance().Handle())
{
	CheckRequiredExtensions(_physicalDevice, requiredExtensions);

	uint32_t QueueFamilyuCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &QueueFamilyuCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamily_;
	if (QueueFamilyuCount)
	{
		queueFamily_.resize(QueueFamilyuCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &QueueFamilyuCount, queueFamily_.data());
	}
	else
	{
		throw std::logic_error("Unable find Device Qeueue Family Propertie");

	}
	const auto graphicsFamily = FindQueue(queueFamily_, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
	const auto computeFamily = FindQueue(queueFamily_, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
	const auto transferFamily = FindQueue(queueFamily_, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT);

	const auto presentFamily = std::find_if(queueFamily_.begin(), queueFamily_.end(), [&](const VkQueueFamilyProperties& queueFamily) {
		VkBool32 presentSupport = false;
		const uint32_t i = static_cast<uint32_t>(&*queueFamily_.cbegin() - &queueFamily);
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, surface.Handle(), &presentSupport);
		return queueFamily.queueCount > 0 && presentSupport;
		});
	if (presentFamily == queueFamily_.end())
	{
		throw (std::runtime_error("found no presentation queue"));
	}
	_graphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamily_.begin());
	_presentFamilyIndex = static_cast<uint32_t>(presentFamily - queueFamily_.begin());
	_transferFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamily_.begin());

	//获取各个Queuefamilies
	const std::set<uint32_t> uniqueQueueFamilies = {
		_graphicsFamilyIndex,
		_presentFamilyIndex,
		_transferFamilyIndex,
	};
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	//启用各个features
	VkPhysicalDeviceFeatures deviceFeatures10 = {
	 .geometryShader = vkFeatures10_.features.geometryShader, // enable if supported
	 .tessellationShader = vkFeatures10_.features.tessellationShader, // enable if supported
	 .sampleRateShading = VK_TRUE,
	 .multiDrawIndirect = VK_TRUE,
	 .drawIndirectFirstInstance = VK_TRUE,
	 .depthBiasClamp = VK_TRUE,
	 .fillModeNonSolid = vkFeatures10_.features.fillModeNonSolid, // enable if supported
	 .samplerAnisotropy = VK_TRUE,
	 .textureCompressionBC = vkFeatures10_.features.textureCompressionBC, // enable if supported
	 .vertexPipelineStoresAndAtomics = vkFeatures10_.features.vertexPipelineStoresAndAtomics, // enable if supported
	 .fragmentStoresAndAtomics = VK_TRUE,
	 .shaderImageGatherExtended = VK_TRUE,
	 .shaderInt64 = vkFeatures10_.features.shaderInt64, // enable if supported
	};
	VkPhysicalDeviceVulkan11Features deviceFeatures11 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = &deviceFeatures10,
		.storageBuffer16BitAccess = VK_TRUE,
		.samplerYcbcrConversion = vkFeatures11_.samplerYcbcrConversion, // enable if supported
		.shaderDrawParameters = VK_TRUE,
	};
	VkPhysicalDeviceVulkan12Features deviceFeatures12 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = &deviceFeatures11,
		.drawIndirectCount = vkFeatures12_.drawIndirectCount, // enable if supported
		.storageBuffer8BitAccess = vkFeatures12_.storageBuffer8BitAccess, // enable if supported
		.uniformAndStorageBuffer8BitAccess = vkFeatures12_.uniformAndStorageBuffer8BitAccess, // enable if supported
		.shaderFloat16 = vkFeatures12_.shaderFloat16, // enable if supported
		.descriptorIndexing = VK_TRUE,
		.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
		.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
		.descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
		.descriptorBindingPartiallyBound = VK_TRUE,
		.descriptorBindingVariableDescriptorCount = VK_TRUE,
		.runtimeDescriptorArray = VK_TRUE,
		.scalarBlockLayout = VK_TRUE,
		.uniformBufferStandardLayout = VK_TRUE,
		.hostQueryReset = vkFeatures12_.hostQueryReset, // enable if supported
		.timelineSemaphore = VK_TRUE,
		.bufferDeviceAddress = VK_TRUE,
		.vulkanMemoryModel = vkFeatures12_.vulkanMemoryModel, // enable if supported
		.vulkanMemoryModelDeviceScope = vkFeatures12_.vulkanMemoryModelDeviceScope, // enable if supported
	};
	VkPhysicalDeviceVulkan13Features deviceFeatures13 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.pNext = &deviceFeatures12,
		.subgroupSizeControl = VK_TRUE,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE,
		.maintenance4 = VK_TRUE,
	};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nextDeviceFeatures;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures10;
	createInfo.enabledLayerCount = static_cast<uint32_t>(_surface.getInstance().GetValidationLayers().size());
	createInfo.ppEnabledLayerNames = _surface.getInstance().GetValidationLayers().data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	Check(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device), "create logical device");

	_debugUtils.SetDevice(_device);
	          
	vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, _presentFamilyIndex, 0, &_presentQueue);
	vkGetDeviceQueue(_device, _transferFamilyIndex, 0, &_transferQueue);
	
	_physicalDeviceDriverProerties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
	_physicalDeviceDriverProerties.pNext = nullptr;
	_physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	_physicalDeviceProperties2.pNext = &_physicalDeviceDriverProerties;

	
	vkGetPhysicalDeviceProperties2(_physicalDevice, &_physicalDeviceProperties2);
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_memProperties);
	
	const uint32_t apiVersion = _physicalDeviceProperties2.properties.apiVersion;
	LLOGL("Vulakn physical device: %s\n", _physicalDeviceProperties2.properties.deviceName);
	LLOGL("                 API version : %i.%i.%i.%i\n", 
		VK_API_VERSION_MAJOR(apiVersion),							
		VK_API_VERSION_MINOR(apiVersion),							
		VK_API_VERSION_PATCH(apiVersion),							
		VK_API_VERSION_VARIANT(apiVersion));
	LLOGL("                 Driver info : %s %s\n", _physicalDeviceDriverProerties.driverName, _physicalDeviceDriverProerties.driverInfo);

}

vulkan::Device::~Device()
{
	if (_device != nullptr) {
		vkDestroyDevice(_device, nullptr);
		_device = nullptr;
	}
}

void vulkan::Device::WaitIdle() const
{
	Check(vkDeviceWaitIdle(_device), "wait for device idle");
}

void vulkan::Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) 
{
	uint32_t extensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, nullptr);
	if (extensionsCount)
	{
		_availableExtensions.resize(extensionsCount);
	}
	vkEnumerateDeviceExtensionProperties(_physicalDevice, static_cast<const char*>(nullptr), &extensionsCount, _availableExtensions.data());
	std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& extension : _availableExtensions)
	{
		required.erase(extension.extensionName);
	}
	if (!required.empty())
	{
		bool first = true;
		std::string extensions;
		for (const auto& extension : required)
		{
			if (!first)
			{
				extensions += ", ";
			}
			extensions += extension;
			first = false;
		}
		throw (std::runtime_error("missing required extensions: " + extensions));
	}
}

vulkan::SwapChain::SwapChain(const Device& device, VkPresentModeKHR presentationMode) : _physicalDevice(device.PhysicalDevice()), _device(device)
{
	CreateSwapChain(presentationMode);
}

void vulkan::SwapChain::CreateSwapChain(VkPresentModeKHR presentationMode)
{
	const auto details = QuerySwapChainSupport(_physicalDevice, _device.VulkanSurface().Handle());
	if (details.Formats.empty() || details.PresentModes.empty())
	{
		throw std::runtime_error("empty swap chain support");
	}

	const auto& surface = _device.VulkanSurface();
	const auto& window = surface.getInstance().getWindow();

	const auto surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
	const auto actualPresentMode = ChooseSwapPresentMode(details.PresentModes, presentationMode);
	const auto extent = ChooseSwapExtent(window, details.Capabilities);
	const auto imageCount = ChooseImageCount(details.Capabilities);

	VkSwapchainCreateInfoKHR createInfo = {
	.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	.surface = surface.Handle(),
	.minImageCount = imageCount,
	.imageFormat = surfaceFormat.format,
	.imageColorSpace = surfaceFormat.colorSpace,
	.imageExtent = extent,
	.imageArrayLayers = 1,
	.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	.preTransform = details.Capabilities.currentTransform,
	.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	.presentMode = actualPresentMode,
	.clipped = VK_TRUE,
	.oldSwapchain = nullptr };

	//如果渲染和呈现用的是同一个队列族，用 独占（EXCLUSIVE） 模式，性能最好。
	//如果它们在不同的队列族，就用 并发（CONCURRENT） 模式，并把两条队列族索引都告诉 Vulkan，让它自动帮你管理图像的访问与同步。
	if (_device.GraphicsFamilyIndex() != _device.PresentFamilyIndex())
	{
		uint32_t queueFamilyIndices[] = { _device.GraphicsFamilyIndex(), _device.PresentFamilyIndex() };
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	Check(vkCreateSwapchainKHR(_device.Handle(), &createInfo, nullptr, &_swapChain), "create swap chain!");

	uint32_t vkimageCount;
	vkGetSwapchainImagesKHR(_device.Handle(), _swapChain, &vkimageCount, nullptr);
	if (vkimageCount)
	{
		_images.resize(vkimageCount);
		vkGetSwapchainImagesKHR(_device.Handle(), _swapChain, &vkimageCount, _images.data());
	}
	else
	{
		throw std::runtime_error("unable find vk image");
	}
	_imageViews.resize(_images.size());
	for (size_t i = 0; i < _imageViews.size(); i++)
	{
		VkImageViewCreateInfo createInfo{ };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			createInfo.image = _images[i],
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D,
			createInfo.format = surfaceFormat.format,
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		Check(vkCreateImageView(_device.Handle(), &createInfo, nullptr, &_imageViews[i]), "Create image view");
	}

	const auto& debugUtils = _device.DebugUtils();

	for (size_t i = 0; i != _images.size(); i++)
	{
		debugUtils.SetObjectName(_images[i], ("Swapchain Image #" + std::to_string(i)).c_str());
		debugUtils.SetObjectName(_imageViews[i], ("Swapchain ImageView #" + std::to_string(i)).c_str());
	}

	_format = surfaceFormat.format;
	_extent = extent;
}

void vulkan::SwapChain::CreateFrameBuffer(VkRenderPass renderPass)
{
	_swapChainFramebuffers.resize(_imageViews.size());
	auto ImageViewSize = _imageViews.size();
	for (size_t i = 0; i < ImageViewSize; i++)
	{
		VkImageView attachments[] = {
			_imageViews[i]
		};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _extent.width;
		framebufferInfo.height = _extent.height;
		framebufferInfo.layers = 1;
		Check(vkCreateFramebuffer(_device.Handle(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]), "Create framebuffer!");

	}
}

void vulkan::SwapChain::CleanUpSwapChain()
{
	for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(_device.Handle(), _swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < _imageViews.size(); i++) {
		vkDestroyImageView(_device.Handle(), _imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(_device.Handle(), _swapChain, nullptr);
	_swapChain = nullptr;
}

vulkan::SwapChain::~SwapChain()
{
	

	if (_swapChain != nullptr)
	{
		vkDestroySwapchainKHR(_device.Handle(), _swapChain, nullptr);
		_swapChain = nullptr;
	}
}

vulkan::SwapChain::SupportDetails vulkan::SwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	SupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
	if (surfaceFormatCount)
	{
		details.Formats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, details.Formats.data());
	}
	uint32_t surfacePresentModesCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModesCount, nullptr);
	if (surfacePresentModesCount)
	{
		details.PresentModes.resize(surfacePresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModesCount, details.PresentModes.data());
	}
	
	return details;
}

VkSurfaceFormatKHR vulkan::SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}
	throw std::runtime_error("found no suitable surface format");
}

VkPresentModeKHR vulkan::SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode)
{
	switch (presentMode)
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
		{
			return presentMode;
		}
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
		{
			return presentMode;
		}
		break;
	case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
		break;
	case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
		break;
	case VK_PRESENT_MODE_FIFO_LATEST_READY_EXT:
		break;
	case VK_PRESENT_MODE_MAX_ENUM_KHR:
		break;
	default:
		throw std::out_of_range("unknow present mode");
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vulkan::SwapChain::ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	VkExtent2D actualExtent = VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

uint32_t vulkan::SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
	uint32_t imageCount = std::max(2u, capabilities.minImageCount);
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}
	return imageCount;
}

vulkan::RenderPass::RenderPass(const SwapChain& swapchain) : _swapChain(swapchain)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _swapChain.GetSwaphchainImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//关于stencil data的操作 深度缓冲等等
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//在swapchian 里面的image需要被转换成适合的layouts
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//创建subpass，一个render pass有一个或多个subp ass，用于存储渲染操作
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//这里指向我们的subpass，从外部进来，转换到我们的subpass的layout
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//开始创建renderPass
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	Check(vkCreateRenderPass(_swapChain.GetDevice().Handle(), &renderPassInfo, nullptr, &_renderPass), "Create Render Pass");
}

vulkan::RenderPass::~RenderPass()
{

}

void vulkan::RenderPass::Destroy()
{
	vkDestroyRenderPass(_swapChain.GetDevice().Handle(), _renderPass, nullptr);
}


/////////////////////////////////////////////////////////////////////////GRAPHIC PIPELINE//////////////////////////////////////////////////////////////

vulkan::GraphicPipeline::GraphicPipeline(const std::unordered_map<std::string, asset::shader>& shaders, VkDevice device, const SwapChain& swapchain, const RenderPass& renderpass)
	: _shaders(shaders), _device(device), _swapChain(swapchain),_renderPass(renderpass)
{
	createPipelineCache();
}

void vulkan::GraphicPipeline::Destroy(std::string pipelineName)
{

	
}

VkPipeline vulkan::GraphicPipeline::GetGraphicsPipeline(std::string pipelineName)
{
	return utils::findInMap(_pipelines, pipelineName);
}



VkPipelineLayout vulkan::GraphicPipeline::GetGraphicsPipelineLayout(std::string piplineLayoutName)
{
	return utils::findInMap(_pipelineLayouts, piplineLayoutName);
}

vulkan::GraphicPipeline::~GraphicPipeline()
{
	for (auto& x : _pipelines) {
		vkDestroyPipeline(_device, x.second, nullptr);
	}
	for (auto& x : _pipelineLayouts) {
		vkDestroyPipelineLayout(_device, x.second, nullptr);
	}
	vkDestroyPipelineCache(_device, _pipelineCache, nullptr);
	
	
}

VkShaderModule vulkan::GraphicPipeline::CreateShaderModule(const std::vector<char>& code)
{
	
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	Check(vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule), "Create shader Module");
	return shaderModule;
	
}

void vulkan::GraphicPipeline::createPipelineCache()
{
	VkPipelineCacheCreateInfo cacheInfo{};
	cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	cacheInfo.initialDataSize = 0;
	cacheInfo.pInitialData = nullptr;
	Check(vkCreatePipelineCache(_device, &cacheInfo, nullptr, &_pipelineCache), "Create Pipeline Cache!");
}

void vulkan::GraphicPipeline::CreateGraphicsPipeline(std::string pipelineName,std::string pipelineLayoutName, const asset::shader& shaders, VkRenderPass renderPass)
{
	//auto it = _shaders.find(pipelineName);
	//if (it == _shaders.end()
	//	|| it->second.fragmentShader.empty()
	//	|| it->second.vertexShader.empty())
	//{
	//	std::cerr << "shader file " << pipelineName << " is not exist." << std::endl;
	//	return;
	//}
	graphicsPipelineCreateInfoPack createinfo;
	VkPipeline pipeline;
	auto it = _pipelineLayouts.find(pipelineLayoutName);
	if (it == _pipelineLayouts.end()) {
		throw std::runtime_error("Unable to find the required pipelineLayout, please use CreatePipelineLayout to create the asscociate layout first");
	}
	PipelineEntry entry;
	//创建Shader Module
	VkShaderModule vertShaderModule = CreateShaderModule(shaders.vertexShader);
	VkShaderModule fragShaderModule = CreateShaderModule(shaders.fragmentShader);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	createinfo.shaderStages.emplace_back(vertShaderStageInfo);
	createinfo.shaderStages.emplace_back(fragShaderStageInfo);

	//顶点输入，用来输入到pipeline中的vertex shader的vertex信息
	//主要两个作用， 绑定顶点中的间隔的距离，还有他们属性的类型以及偏移
	//TODO 抽象出来，要弄几个弄几个
	
	getPipelineVertexInputState({ VertexComponent::Position, VertexComponent::Color, VertexComponent::UV}, createinfo);

	//规定图元类型，如三角形，线，点等
	createinfo.inputAssemblyStateCi.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createinfo.inputAssemblyStateCi.primitiveRestartEnable = VK_FALSE;


	//创建动态状态
	createinfo.dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
	createinfo.dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

	
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)_swapChain.GetSwapchainExtent().width;
	viewport.height = (float)_swapChain.GetSwapchainExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	createinfo.viewports.push_back(viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _swapChain.GetSwapchainExtent();

	createinfo.scissors.push_back(scissor);

	//光栅器
	//The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader. 
	//It also performs depth testing, face culling and the scissor test,
	//and it can be configured to output fragments that fill entire polygons or just the edges (wireframe rendering). All this is
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	
	createinfo.rasterizationStateCi = rasterizer;


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	createinfo.multisampleStateCi = multisampling;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	createinfo.colorBlendAttachmentStates.push_back(colorBlendAttachment);

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	createinfo.colorBlendStateCi = colorBlending;

	//VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//pipelineLayoutInfo.setLayoutCount = 1; // Optional
	//pipelineLayoutInfo.pSetLayouts = &descriptorLayout; // Optional
	//pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	//pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	//
	//std::string pipelineLayoutNameInfo = "Create pipeline for ";
	//pipelineLayoutNameInfo += pipelineName;
	//Check(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &entry.layout), pipelineLayoutNameInfo.c_str());
	
	createinfo.createInfo.layout = it->second;
	createinfo.createInfo.renderPass = renderPass;
	createinfo.createInfo.stageCount = 2;
	createinfo.UpdateAllArrays();
	std::string pipelineNameInfo = "Create pipeline for ";
	pipelineNameInfo += pipelineName;
	
	Check(vkCreateGraphicsPipelines(_device, nullptr, 1,&createinfo.createInfo, nullptr, &pipeline), pipelineNameInfo.c_str());
	_pipelines.emplace(pipelineName, pipeline);
	entry.pipeline = pipelineName;
	entry.layout = pipelineLayoutName;
	_pipelineEntrys[pipelineName] = entry;

	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);

}

void vulkan::GraphicPipeline::createPipelineLayout(std::string name, VkDescriptorSetLayout descriptorLayout)
{
	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = &descriptorLayout; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	std::string debugName = "Create " + name + "Pipeline Layout";
	Check(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &pipelineLayout), debugName.c_str());
	_pipelineLayouts.emplace(name, pipelineLayout);

}



/////////////////////////////////////////////////////////////////////////COMMAND POOL//////////////////////////////////////////////////////////////
vulkan::CommandPoolManager::CommandPoolManager(const Device& device) : _device(device)
{
	Init();
}

vulkan::CommandPoolManager::~CommandPoolManager()
{
	for (auto& pool : _commandPools)
	{
		vkDestroyCommandPool(_device.Handle(), pool.second, nullptr);
	}
}

void vulkan::CommandPoolManager::CreateCommandPool(QueueFamily family)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	switch (family)
	{
	case QueueFamily::GRAPHIC:
		poolInfo.queueFamilyIndex = _device.GraphicsFamilyIndex();
		break;
	case QueueFamily::COMPUTE:
		break;
	case QueueFamily::TRANSFER:
		poolInfo.queueFamilyIndex = _device.TransferFamilyIndex();
		break;
	default:
		break;
	}
	Check(vkCreateCommandPool(_device.Handle(), &poolInfo, nullptr, &_commandPools[family]), "Create Command pool");
}

void vulkan::CommandPoolManager::FreeCommandBuffer(QueueFamily family, uint32_t count, const VkCommandBuffer& buffer)
{
	vkFreeCommandBuffers(_device.Handle(), _commandPools[family], count, &buffer);
}

void vulkan::CommandPoolManager::Init()
{
	CreateCommandPool(QueueFamily::GRAPHIC);
	CreateCommandPool(QueueFamily::TRANSFER);
}

vulkan::DescriptorLayoutManager::DescriptorLayoutManager(const Device& deivce) : _device(deivce)
{
	

}

vulkan::DescriptorLayoutManager::~DescriptorLayoutManager()
{
	for (auto& entry : _LayoutCache)
	{
		vkDestroyDescriptorSetLayout(_device.Handle(), entry.second, nullptr);
	}
}

void vulkan::DescriptorLayoutManager::CreateDescriptorSetLayout(LayoutConfig config)
{
	auto it = _LayoutCache.find(config);
	if (it == _LayoutCache.end()){
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = config.bindingCounts;
		layoutInfo.pBindings = config.bindings.data();
		Check(vkCreateDescriptorSetLayout(_device.Handle(), &layoutInfo, nullptr, &_LayoutCache[config]),"Create DescriptorSetLayout");
	}
	
}

vulkan::DescriptorPoolManager::DescriptorPoolManager(const Device& deivce) : _device(deivce)
{
	CreateGlobalDescriptorPool();
	CreatePerFrameDescriptorPool();
}

vulkan::DescriptorPoolManager::~DescriptorPoolManager()
{
	vkDestroyDescriptorPool(_device.Handle(), _GlobalPool, nullptr);
	for (auto& pool: _PerFramePool)
	{
		vkDestroyDescriptorPool(_device.Handle(), pool, nullptr);
	}
	
}

void vulkan::DescriptorPoolManager::CreateGlobalDescriptorPool()
{
	VkDescriptorPoolSize globalPoolSizes[] = {
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50 }, };
	VkDescriptorPoolCreateInfo globalPoolInfo{};
	globalPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	globalPoolInfo.poolSizeCount = 1;
	globalPoolInfo.pPoolSizes = globalPoolSizes;
	globalPoolInfo.maxSets = 50;

	Check(vkCreateDescriptorPool(_device.Handle(), &globalPoolInfo, nullptr, &_GlobalPool), "Create GlobalDescriptorPool");
}

void vulkan::DescriptorPoolManager::CreatePerFrameDescriptorPool()
{
	_PerFramePool.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 20},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20}
		};
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 20;

		Check(vkCreateDescriptorPool(_device.Handle(), &poolInfo, nullptr, &_PerFramePool[i]), "Create per frame descriptor pool");
	}
}

void vulkan::DescriptorPoolManager::CreatePreFrameDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts){
	_hardCodeDescriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _PerFramePool[i];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();
		Check(vkAllocateDescriptorSets(_device.Handle(), &allocInfo, _hardCodeDescriptorSet.data()), "Allocate Descriptor set");
	}

}

vulkan::BufferManager::BufferManager(const Device& deivce, CommandPoolManager& commandPools) : device(deivce), commandPools(commandPools)
{

}

vulkan::BufferManager::~BufferManager()
{
}

void vulkan::BufferManager::CreateIndexBuffer1(std::vector<uint32_t>& indiceData, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkDeviceSize bufferSize = sizeof(indiceData[0]) * indiceData.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferSize,
		&stagingBuffer,
		&stagingBufferMemory,
		indiceData.data());
	createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		&buffer,
		&memory);
	copyBuffer(stagingBuffer, buffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);
}

void vulkan::BufferManager::CreateVertexBuffer1(std::vector<Vertex1>& vertexData, VkBuffer& buffer, VkDeviceMemory& memory)
{
	VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();
	//创建一个临时缓冲区，用于直接传输顶点数据至GPU
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferSize,
		&stagingBuffer,
		&stagingBufferMemory,
		vertexData.data());
	createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		&buffer,
		&memory);
	copyBuffer(stagingBuffer, buffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);
}

void vulkan::BufferManager::CreateVulkanImageBuffer(Image image_data, VkImageLayout& image_layout, VkImage& image, VkDeviceMemory& memory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkDeviceSize image_size = image_data.texWidth * image_data.texHeight * 4;
	createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		image_size,
		&stagingBuffer,
		&stagingBufferMemory,
		image_data.pixel);
	createImageBuffer(image_data.texWidth, image_data.texHeight,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&image,
		&memory);

	image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, image_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, QueueFamily::TRANSFER);
	//transitionImageLayout1(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(image_data.texWidth), static_cast<uint32_t>(image_data.texHeight), QueueFamily::TRANSFER);
	//copyBufferToImage1(stagingBuffer, image, static_cast<uint32_t>(image_data.texWidth), static_cast<uint32_t>(image_data.texHeight));
	transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, image_layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, QueueFamily::GRAPHIC);
	//transitionImageLayout1(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkDestroyBuffer(device.Handle(), stagingBuffer, nullptr);
	vkFreeMemory(device.Handle(), stagingBufferMemory, nullptr);

}

void vulkan::BufferManager::createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferInfo.queueFamilyIndexCount = 2;
	uint32_t families[] = { device.GraphicsFamilyIndex(), device.TransferFamilyIndex() };
	bufferInfo.pQueueFamilyIndices = families;
	Check(vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, buffer), "Create buffer!");
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.Handle(), *buffer, &memRequirements);
	VkMemoryAllocateInfo alloInfo{};
	alloInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloInfo.allocationSize = memRequirements.size;
	alloInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);
	//这玩意光追管线得开，让shader直接访问gpu内存
	VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
	if (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
		allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
		allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
		alloInfo.pNext = &allocFlagsInfo;
	}
	Check(vkAllocateMemory(device.Handle(), &alloInfo, nullptr, memory), "Allocate memory");

	if (data != nullptr) {
		void* mapped;
		Check(vkMapMemory(device.Handle(), *memory, 0, size, 0, &mapped),"Map memory");
		memcpy(mapped, data, size);
		if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
			VkMappedMemoryRange mappedRange{};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.offset = 0;
			mappedRange.size = size;
			mappedRange.memory = *memory;
			vkFlushMappedMemoryRanges(device.Handle(), 1, &mappedRange);
		}
		vkUnmapMemory(device.Handle(), *memory);
	}
	
	Check(vkBindBufferMemory(device.Handle(), *buffer, *memory, 0), "Bind Memory");
}

void vulkan::BufferManager::createImageBuffer(uint32_t width,
	uint32_t height, VkFormat format, VkImageTiling tiling, 
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Check(vkCreateImage(device.Handle(), &imageInfo, nullptr,image), "create vk image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.Handle(), *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	Check(vkAllocateMemory(device.Handle(), &allocInfo, nullptr, imageMemory), "allocate image memory");

	vkBindImageMemory(device.Handle(), *image, *imageMemory, 0);


	

}

void vulkan::BufferManager::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);
	VkImageMemoryBarrier barrier{};
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlagBits destinationStage;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition");
	};
	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	if (family == QueueFamily::GRAPHIC) {
		flushCommandBuffer(cmdBuffer, device.GraphicsQueue(), commandPools.GetCommandPool(family), true);
	}
	else
	{
		flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);

	}

}

void vulkan::BufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, & copyRegion);
	flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);

}

void vulkan::BufferManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, QueueFamily family)
{
	VkCommandBuffer cmdBuffer = createInstantCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPools.GetCommandPool(family), 1, true);
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0,0,0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	vkCmdCopyBufferToImage(
		cmdBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	flushCommandBuffer(cmdBuffer, device.TransferQueue(), commandPools.GetCommandPool(family), true);
}

VkCommandBuffer vulkan::BufferManager::createInstantCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, uint32_t bufferCount, bool begin)
{
	
	VkCommandBuffer cmdBuffer;
	VkCommandBufferAllocateInfo cmdBufferAllocateInfo{};
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.commandPool = pool;
	cmdBufferAllocateInfo.commandBufferCount = bufferCount;
	cmdBufferAllocateInfo.level = level;
	Check(vkAllocateCommandBuffers(device.Handle(), &cmdBufferAllocateInfo, &cmdBuffer),"Allocate CommandBuffer");
	if (begin) {
		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		Check(vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo), "Begin record buffer");
	}
	return cmdBuffer;

}

uint32_t vulkan::BufferManager::findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) const
{
	VkPhysicalDeviceMemoryProperties memProperty= device.GetPhyDeviceMemProperty();
	for (uint32_t i = 0; i < memProperty.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memProperty.memoryTypes[i].propertyFlags & properties) == properties) {
				if (memTypeFound) {
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}
	if (memTypeFound) {
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Cound not find a matching memory type");
	}

}

VkImageView vulkan::BufferManager::CreateImageView(VkImage image, VkFormat format)
{
	
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		Check(vkCreateImageView(device.Handle(), &viewInfo, nullptr, &imageView), "Create imageView");
		return imageView;
	

}

void vulkan::BufferManager::CreateTextureSampler(VkSampler& sampler)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	
	samplerInfo.maxAnisotropy = device.GetPhyDeviceProperty().properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	Check(vkCreateSampler(device.Handle(), &samplerInfo, nullptr, &sampler), "Create Sampler");

}



void vulkan::BufferManager::DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory)
{
	vkFreeMemory(device.Handle(), memory, nullptr);
	vkDestroyBuffer(device.Handle(), buffer, nullptr);
}

void vulkan::BufferManager::DestroyVkImage(VkImage image, VkDeviceMemory memory)
{
	vkDestroyImage(device.Handle(), image, nullptr);
	vkFreeMemory(device.Handle(), memory, nullptr);
}

void vulkan::BufferManager::DestroyVkImageView(VkImageView imageview)
{
	vkDestroyImageView(device.Handle(), imageview,nullptr);
}

void vulkan::BufferManager::DestroySampler(VkSampler sampler)
{
	vkDestroySampler(device.Handle(), sampler, nullptr);
}

void vulkan::BufferManager::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device.Handle(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.Handle(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device.Handle(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device.Handle(), image, imageMemory, 0);
}

VkCommandBuffer vulkan::BufferManager::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPools.GetCommandPool(QueueFamily::GRAPHIC);
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device.Handle(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void vulkan::BufferManager::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device.GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device.GraphicsQueue());

	vkFreeCommandBuffers(device.Handle(), commandPools.GetCommandPool(QueueFamily::GRAPHIC), 1, &commandBuffer);
}

void vulkan::BufferManager::copyBuffer1(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void vulkan::BufferManager::transitionImageLayout1(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	endSingleTimeCommands(commandBuffer);
}

void vulkan::BufferManager::copyBufferToImage1(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	endSingleTimeCommands(commandBuffer);
}

void vulkan::BufferManager::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free)
{
	if (commandBuffer == VK_NULL_HANDLE) {
		return;
	}
	Check(vkEndCommandBuffer(commandBuffer), "end Command buffer");
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	VkFence fence;
	Check(vkCreateFence(device.Handle(), &fenceCreateInfo, nullptr, &fence), "Create fence for flush command buffer ");
	Check(vkQueueWaitIdle(queue), "wait queue idle");
	Check(vkQueueSubmit(queue, 1, &submitInfo, fence), "Submit to queue");
	Check(vkWaitForFences(device.Handle(), 1, &fence, VK_TRUE, 100000000000), "Wait for finished excuting commandbuffer");
	vkDestroyFence(device.Handle(), fence, nullptr);
	if (free) {
		
		vkFreeCommandBuffers(device.Handle(), pool, 1, &commandBuffer);
	}


}

vulkan::VulkanResouceManager::VulkanResouceManager(BufferManager& bufferManager, const asset::AssetManager& assetManager) : 
	_BufferManager(bufferManager), _assetMnanger(assetManager)
{

}

vulkan::VulkanResouceManager::~VulkanResouceManager()
{
	for (auto& x : _renderObjects) {
		_BufferManager.DestroyBuffer(x.second.indiceBuffer, x.second.indicememory);
		_BufferManager.DestroyBuffer(x.second.vertexBuffer, x.second.vertexmemory);
		for (auto& x : x.second.textures) {
			_BufferManager.DestroyVkImageView(x.view);
			_BufferManager.DestroyVkImage(x.image, x.deviceMemory);
			_BufferManager.DestroySampler(x.sampler);
		}
	}
}

void vulkan::VulkanResouceManager::ConstructVulkanRenderObject(
	std::string name,  
	VkPipeline pipeline, 
	VkPipelineLayout layout, std::string raw_model_name, 
	std::vector<std::string> textureFiles)
{
	ModelData modeldata = _assetMnanger.getModelDataByName(raw_model_name);
	VulkanRenderObject renderObject;

	renderObject.name = name;
	for (auto& x : modeldata.meshes) {
		_BufferManager.CreateVertexBuffer1(x.vertices, renderObject.vertexBuffer, renderObject.vertexmemory);
		_BufferManager.CreateIndexBuffer1(x.indices, renderObject.indiceBuffer, renderObject.indicememory);
		renderObject.indiceCounts.push_back(x.indices.size());
	}

	renderObject.pipeline = pipeline;
	renderObject.Pipelinelayout = layout;
	
	for (const auto& x : textureFiles) {
		const Image& image = _assetMnanger.getImageDataByName(x);

		VulkanTexture texture;
		texture.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_BufferManager.CreateVulkanImageBuffer(image, texture.imageLayout, texture.image, texture.deviceMemory);
		CreateTextureImageView(texture.view, texture.image);
		_BufferManager.CreateTextureSampler(texture.sampler);
		renderObject.textures.push_back(texture);
	}
	
	
	_renderObjects[name] = renderObject;
}

const vulkan::VulkanRenderObject& vulkan::VulkanResouceManager::GetRenderObject(std::string name)
{
	return utils::findInMap(_renderObjects, name);
}



void vulkan::VulkanResouceManager::ConstructVulkanRenderObject()
{
	
	//1.iterate the raw data sources
	
	for (const auto& x : _assetMnanger.getModelDatas()) {
		VulkanRenderObject object;
		object.name = x.first;

	}
	//2.Use the data in raw datas to create the buffer for the vertex and index
	//3.Get the image datas to create Textures for future use
	//4. animate if any
	//5 skin if any
	//6 store it into read to render object
}

void vulkan::VulkanResouceManager::CreateTextureImageView(VkImageView& imageview, VkImage image)
{
	imageview = _BufferManager.CreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB);

}

