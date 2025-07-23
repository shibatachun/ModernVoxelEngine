#pragma once
#include "VulkanCommon.h"
namespace vulkan {


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
		std::vector<const char*>					validationLayers_;
		VULKAN_HANDLE(VkInstance, instance_)
		std::vector<VkExtensionProperties>			extension_;
		std::vector<VkLayerProperties>				layers_;
		std::vector<VkPhysicalDevice>				physicalDevices_;
	



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

		uint32_t GraphicsFamilyIndex() const { return _graphicsFamilyIndex; }
		uint32_t PresentFamilyIndex() const { return _presentFamilyIndex; }
		uint32_t TransferFamilyIndex() const { return _transferFamilyIndex; }
		VkPhysicalDevice PhysicalDevice() const { return _physicalDevice; }
		const Surface& VulkanSurface() const { return _surface; }
		VkQueue GraphicsQueue() const { return _graphicsQueue; }
		VkQueue PresentQueue() const { return _presentQueue; }
		VkQueue TransferQueue() const { return _transferQueue; }
		const DebugUtils& DebugUtils() const { return _debugUtils; }
		void WaitIdle() const;
	private:
		VULKAN_HANDLE(VkDevice, _device)
		const VkPhysicalDevice							_physicalDevice;
		const Surface&									_surface;
		std::vector<VkExtensionProperties>				_availableExtensions;
		class DebugUtils								_debugUtils;
		uint32_t										_graphicsFamilyIndex{};
		uint32_t										_presentFamilyIndex{};
		uint32_t										_transferFamilyIndex{};

		VkQueue											_graphicsQueue{};
		VkQueue											_presentQueue{};
		VkQueue											_transferQueue{};

		VkPhysicalDeviceProperties2						_physicalDeviceProperties2;
		VkPhysicalDeviceDriverProperties				_physicalDeviceDriverProerties;
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
	private:
		VULKAN_HANDLE(VkSwapchainKHR, _swapChain)
		const VkPhysicalDevice										_physicalDevice;
		const class Device&											_device;
		uint32_t													_minImageCount;
		VkPresentModeKHR											_presentMode;
		VkFormat													_format;
		VkExtent2D													_extent;
		std::vector<VkImage>										_images;
		std::vector<VkImageView>									_imageViews;
		std::vector<VkFramebuffer>									_swapChainFramebuffers;
	};

	class RenderPass final {
	public:

		VULKAN_NON_COPIABLE(RenderPass)
		RenderPass(const SwapChain& swapchain);
		~RenderPass();
		const VkRenderPass GetRenderPass() const { return _renderPass; }
		void Destroy();

	private:
		const SwapChain&											_swapChain;
		VkRenderPass												_renderPass;

	};

	class GraphicPipeline final {
	
	public:
		VULKAN_NON_COPIABLE(GraphicPipeline)
		GraphicPipeline(
			const std::unordered_map<std::string, asset::shader>& shaders, 
			VkDevice device,
			const  SwapChain& swapchain, 
			const RenderPass& renderpass);
		void Destroy(std::string pipelineName);
		PipelineEntry GetGraphicsPipeline(std::string pipelineName);
		void CreateGraphicsPipeline(std::string pipelineName, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout);
		~GraphicPipeline();
	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		
	private:
		
		std::unordered_map<std::string, PipelineEntry>				_pipelineEntrys;
		const std::unordered_map<std::string, asset::shader>&		_shaders;
		VkDevice													_device;
		const SwapChain&											_swapChain;
		const RenderPass&											_renderPass;
	};

	class CommandPoolManager final {
	public:
		VULKAN_NON_COPIABLE(CommandPoolManager)
		CommandPoolManager(const Device& device);
		~CommandPoolManager();
		void CreateCommandPool(QueueFamily);
		VkCommandPool GetCommandPool(QueueFamily family) { return _commandPools[family]; };
		void FreeCommandBuffer(QueueFamily, uint32_t, const VkCommandBuffer&);
	private:
		void Init();
		std::unordered_map<QueueFamily, VkCommandPool>				_commandPools;
		const Device&												_device;


	};

	class DescriptorLayoutManager final {
	public:
		VULKAN_NON_COPIABLE(DescriptorLayoutManager)
		DescriptorLayoutManager(const Device& deivce);
		VkDescriptorSetLayout GetDescriptorSetLayout(LayoutConfig config) { return _LayoutCache[config]; };
		~DescriptorLayoutManager();
		void CreateDescriptorSetLayout(LayoutConfig config);
	private:
		std::unordered_map<LayoutConfig, VkDescriptorSetLayout, LayoutConfigHash>				_LayoutCache;
		const Device&																			_device;
	};

	class DescriptorPoolManager final {
	public:
		VULKAN_NON_COPIABLE(DescriptorPoolManager)
		DescriptorPoolManager(const Device& deivce);
		~DescriptorPoolManager();
		void CreateGlobalDescriptorPool();
		void CreatePerFrameDescriptorPool();
		void CreatePreFrameDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts);
		std::vector<VkDescriptorSet> GetHardCodedDescriptorSet() { return _hardCodeDescriptorSet; };

	private:
		VkDescriptorPool														_GlobalPool;
		std::vector<VkDescriptorPool>											_PerFramePool;
		std::vector<VkDescriptorSet>											_hardCodeDescriptorSet;
		std::unordered_map<VkDescriptorSetLayout, std::vector<VkDescriptorSet>> _descriptorSetCache;
		const Device&															_device;

	};
		
}


