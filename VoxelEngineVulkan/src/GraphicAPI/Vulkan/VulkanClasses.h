#pragma once
#include "VulkanCommon.h"
namespace vulkan {

	struct graphicsPipelineCreateInfoPack {
		VkGraphicsPipelineCreateInfo createInfo =
		{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		//Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		std::vector<VkVertexInputBindingDescription> vertexInputBindings;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
		//Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		//Tessellation
		VkPipelineTessellationStateCreateInfo tessellationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
		//Viewport
		VkPipelineViewportStateCreateInfo viewportStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;
		uint32_t dynamicViewportCount = 1;//动态视口/剪裁不会用到上述的vector，因此动态视口和剪裁的个数向这俩变量手动指定
		uint32_t dynamicScissorCount = 1;
		//Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		//Multisample
		VkPipelineMultisampleStateCreateInfo multisampleStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		//Depth & Stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		//Color Blend
		VkPipelineColorBlendStateCreateInfo colorBlendStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
		//Dynamic
		VkPipelineDynamicStateCreateInfo dynamicStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		std::vector<VkDynamicState> dynamicStates;
		//--------------------
		graphicsPipelineCreateInfoPack() {
			SetCreateInfos();
			//若非派生管线，createInfo.basePipelineIndex不得为0，设置为-1
			createInfo.basePipelineIndex = -1;
		}
		//移动构造器，所有指针都要重新赋值
		graphicsPipelineCreateInfoPack(const graphicsPipelineCreateInfoPack& other) noexcept {
			createInfo = other.createInfo;
			SetCreateInfos();

			vertexInputStateCi = other.vertexInputStateCi;
			inputAssemblyStateCi = other.inputAssemblyStateCi;
			tessellationStateCi = other.tessellationStateCi;
			viewportStateCi = other.viewportStateCi;
			rasterizationStateCi = other.rasterizationStateCi;
			multisampleStateCi = other.multisampleStateCi;
			depthStencilStateCi = other.depthStencilStateCi;
			colorBlendStateCi = other.colorBlendStateCi;
			dynamicStateCi = other.dynamicStateCi;

			shaderStages = other.shaderStages;
			vertexInputBindings = other.vertexInputBindings;
			vertexInputAttributes = other.vertexInputAttributes;
			viewports = other.viewports;
			scissors = other.scissors;
			colorBlendAttachmentStates = other.colorBlendAttachmentStates;
			dynamicStates = other.dynamicStates;
			UpdateAllArrayAddresses();
		}
		//Getter，这里我没用const修饰符
		operator VkGraphicsPipelineCreateInfo& () { return createInfo; }
		//Non-const Function
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，并相应改变各个count
		void UpdateAllArrays() {
			createInfo.stageCount = shaderStages.size();
			vertexInputStateCi.vertexBindingDescriptionCount = vertexInputBindings.size();
			vertexInputStateCi.vertexAttributeDescriptionCount = vertexInputAttributes.size();
			viewportStateCi.viewportCount = viewports.size() ? uint32_t(viewports.size()) : dynamicViewportCount;
			viewportStateCi.scissorCount = scissors.size() ? uint32_t(scissors.size()) : dynamicScissorCount;
			colorBlendStateCi.attachmentCount = colorBlendAttachmentStates.size();
			dynamicStateCi.dynamicStateCount = dynamicStates.size();
			UpdateAllArrayAddresses();
		}
	private:
		//该函数用于将创建信息的地址赋值给basePipelineIndex中相应成员
		void SetCreateInfos() {
			createInfo.pVertexInputState = &vertexInputStateCi;
			createInfo.pInputAssemblyState = &inputAssemblyStateCi;
			createInfo.pTessellationState = &tessellationStateCi;
			createInfo.pViewportState = &viewportStateCi;
			createInfo.pRasterizationState = &rasterizationStateCi;
			createInfo.pMultisampleState = &multisampleStateCi;
			createInfo.pDepthStencilState = &depthStencilStateCi;
			createInfo.pColorBlendState = &colorBlendStateCi;
			createInfo.pDynamicState = &dynamicStateCi;
		}
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，但不改变各个count
		void UpdateAllArrayAddresses() {
			createInfo.pStages = shaderStages.data();
			vertexInputStateCi.pVertexBindingDescriptions = vertexInputBindings.data();
			vertexInputStateCi.pVertexAttributeDescriptions = vertexInputAttributes.data();
			viewportStateCi.pViewports = viewports.data();
			viewportStateCi.pScissors = scissors.data();
			colorBlendStateCi.pAttachments = colorBlendAttachmentStates.data();
			dynamicStateCi.pDynamicStates = dynamicStates.data();
		}
	};

	class Instance final {
	public:
		VULKAN_NON_COPIABLE(Instance);
		Instance(GLFWwindow* window);
		~Instance();
		GLFWwindow* getWindow() const;

		const std::vector<const char*>& GetValidationLayers() const { return validationLayers_; }
		const std::vector<VkPhysicalDevice> PhysicalDevices() const { return physicalDevices_; }

	private:
		GLFWwindow* _window;
		uint32_t _vulkanVersion = VK_API_VERSION_1_3;
		std::vector<const char*> validationLayers_;
		VULKAN_HANDLE(VkInstance, instance_)
		std::vector<VkExtensionProperties> extension_;
		std::vector<VkLayerProperties> layers_;
		std::vector<VkPhysicalDevice> physicalDevices_;



	private:
		void GetVulkanExtensions();
		void GetVulkanLayers();
		void GetVulkanPhysicalDevices();

		void CheckVulkanMinimumVersion(const uint32_t minVersion);
		void CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

	};

	class Surface final {
	public:
		VULKAN_NON_COPIABLE(Surface)
		explicit Surface(const Instance& instance);
		~Surface();
		const class Instance& getInstance() const {
			return  instance_;
		}
	private:
		const Instance& instance_;
		VULKAN_HANDLE(VkSurfaceKHR, surface_);
	};

	class DebugUtilsMessenger final {
	public:
		VULKAN_NON_COPIABLE(DebugUtilsMessenger)
		DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
		~DebugUtilsMessenger();

		VkDebugUtilsMessageSeverityFlagBitsEXT Threshold() const { return threshold_; }
	private:
		const Instance& instance_;
		const VkDebugUtilsMessageSeverityFlagBitsEXT threshold_;

		VULKAN_HANDLE(VkDebugUtilsMessengerEXT, messenger_)
	};

	class DebugUtils final {
	public:

		VULKAN_NON_COPIABLE(DebugUtils)

			explicit DebugUtils(VkInstance instance);
		~DebugUtils() = default;


		void SetDevice(VkDevice device)
		{
			device_ = device;
		}

		void SetObjectName(const VkAccelerationStructureKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR); }
		void SetObjectName(const VkBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_BUFFER); }
		void SetObjectName(const VkCommandBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_COMMAND_BUFFER); }
		void SetObjectName(const VkDescriptorSet& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET); }
		void SetObjectName(const VkDescriptorSetLayout& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT); }
		void SetObjectName(const VkDeviceMemory& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DEVICE_MEMORY); }
		void SetObjectName(const VkFramebuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_FRAMEBUFFER); }
		void SetObjectName(const VkImage& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE); }
		void SetObjectName(const VkImageView& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE_VIEW); }
		void SetObjectName(const VkPipeline& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_PIPELINE); }
		void SetObjectName(const VkQueue& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_QUEUE); }
		void SetObjectName(const VkRenderPass& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_RENDER_PASS); }
		void SetObjectName(const VkSemaphore& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SEMAPHORE); }
		void SetObjectName(const VkShaderModule& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SHADER_MODULE); }
		void SetObjectName(const VkSwapchainKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR); }

	private:

		template <typename T>
		void SetObjectName(const T& object, const char* name, VkObjectType type) const
		{
#ifndef NDEBUG
			VkDebugUtilsObjectNameInfoEXT info = {};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			info.pNext = nullptr;
			info.objectHandle = reinterpret_cast<const uint64_t&>(object);
			info.objectType = type;
			info.pObjectName = name;

			Check(vkSetDebugUtilsObjectNameEXT_(device_, &info), "set object name");
#endif
		}

		const PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_;

		VkDevice device_{};
	};

	class Device final {
	public:
		VULKAN_NON_COPIABLE(Device);
		Device(VkPhysicalDevice device,const Surface&, const std::vector<const char*>& requiredExtension, const VkPhysicalDeviceFeatures& deviceFeatures, const void* nextDeviceFeatures);
		~Device();

		uint32_t GraphicsFamilyIndex() const { return graphicsFamilyIndex_; }
		uint32_t PresentFamilyIndex() const { return presentFamilyIndex_; }
		uint32_t TransferFamilyIndex() const { return _transferFamilyIndex; }
		VkPhysicalDevice PhysicalDevice() const { return _physicalDevice; }
		const Surface& VulkanSurface() const { return _surface; }
		VkQueue GraphicsQueue() const { return graphicsQueue_; }
		VkQueue PresentQueue() const { return presentQueue_; }
		VkQueue TransferQueue() const { return _transferQueue; }
		const DebugUtils& DebugUtils() const { return _debugUtils; }
		void WaitIdle() const;
	private:
		const VkPhysicalDevice _physicalDevice;
		const Surface& _surface;
		std::vector<VkExtensionProperties> availableExtensions_;
		VULKAN_HANDLE(VkDevice, device_)
		class DebugUtils _debugUtils;
		uint32_t graphicsFamilyIndex_{};
		uint32_t presentFamilyIndex_{};
		uint32_t _transferFamilyIndex{};

		VkQueue graphicsQueue_{};
		VkQueue presentQueue_{};
		VkQueue _transferQueue{};
	private:
		void CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions);




	};

	class SwapChain final {
	public:
		VULKAN_NON_COPIABLE(SwapChain)
			SwapChain(const Device& device, VkPresentModeKHR presentationMode);
		const VkExtent2D GetSwapchainExtent() const { return _extent; };
		const VkFormat GetSwaphchainImageFormat() const { return _format; };
		std::vector<VkFramebuffer>& GetSwapChainBuffer() { return _swapChainFramebuffers; };
		std::vector<VkImageView>& GetImageViews() { return _imageViews; };
		const Device& GetDevice() const { return _device; };
		void CreateSwapChain(VkPresentModeKHR presentationMode);
		void CreateFrameBuffer(VkRenderPass renderPass);
		void CleanUpSwapChain();
		
		~SwapChain();
	private:
		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities{};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		static SupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode);
		static VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
		static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

		const VkPhysicalDevice _physicalDevice;
		const class Device& _device;

		VULKAN_HANDLE(VkSwapchainKHR, _swapChain)

		uint32_t _minImageCount;
		VkPresentModeKHR _presentMode;
		VkFormat _format;
		VkExtent2D _extent;
		std::vector<VkImage> _images;
		std::vector<VkImageView> _imageViews;

		std::vector<VkFramebuffer>								_swapChainFramebuffers;

	};

	class RenderPass final {
	public:

		VULKAN_NON_COPIABLE(RenderPass)
		RenderPass(const SwapChain& swapchain);
		~RenderPass();
		const VkRenderPass GetRenderPass() const { return _renderPass; }
		void Destroy();

	private:
		const SwapChain& _swapChain;
		VkRenderPass _renderPass;

	};

	class GraphicPipeline final {
	public:
		VULKAN_NON_COPIABLE(GraphicPipeline)
		GraphicPipeline(
			std::unordered_map<std::string, asset::shader> shaders, 
			VkDevice device,
			const  SwapChain& swapchain, 
			const RenderPass& renderpass);
		void Destroy(std::string pipelineName);
		std::unordered_map<std::string, VkPipeline> GetGraphicsPipeline() { return _graphicsPipeline; };
		~GraphicPipeline();
	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		void CreateGraphicsPipeline(std::string pipelineName);
	private:
		std::unordered_map<std::string, VkPipeline> _graphicsPipeline;
		std::unordered_map<std::string, asset::shader>& _shaders;
		std::unordered_map<std::string, VkPipelineLayout> _graphicsPipelineLayout;
		VkDevice _device;
		VkShaderModule _shaderModule;
		const SwapChain& _swapChain;
		const RenderPass& _renderPass;

		
	};
		
}


