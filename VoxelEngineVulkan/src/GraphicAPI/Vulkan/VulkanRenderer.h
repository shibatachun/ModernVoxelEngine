#pragma once
#include "../IRenderer.h"
#include "../../ResourcesManager/AssetManager.h"
#include "VulkanClasses.h"

namespace vulkan
{
	class VulkanRenderer : public IRenderer
	{
	public:
		VULKAN_NON_COPIABLE(VulkanRenderer)
		VulkanRenderer(GLFWwindow* window, VkPresentModeKHR presentMode,asset::AssetManager& assetManager);
		bool Init() override;
		void DrawFrame() override;
		void Cleanup() override;
		API getAPI() const override { return API::VULKAN; }
		
	public:
		bool													_framebufferResized = false;

	private:
		const VkPresentModeKHR _presentMode;
		GLFWwindow* _window;
		asset::AssetManager& _assetManager;
		std::unique_ptr<class vulkan::Instance>					_instance;
		std::unique_ptr<class vulkan::Surface>					_surface;
		std::unique_ptr<class vulkan::DebugUtilsMessenger>		_debugMessenger;
		std::unique_ptr<class vulkan::Device>					_devices;
		std::unique_ptr<class vulkan::SwapChain>				_swapchain;
		std::unique_ptr<class vulkan::GraphicPipeline>			_graphicsPipline;
		std::unique_ptr<class vulkan::RenderPass>				_renderPass;

		
		VkCommandPool											_commandPool;

		std::vector<VkCommandBuffer>							_commandBuffers;
		std::vector<VkSemaphore>								_imageAvailableSemaphores;
		std::vector<VkSemaphore>								_renderFinishedSemaphores;
		std::vector<VkFence>									_inFlightFences;

		VkBuffer												_vertexBuffer;
		VkDeviceMemory											_vertexBufferMemory;
		
		
		uint32_t												_currentFrame = 0;
		
		
	private:
		void SetPhysicalDevices();
		void SetSwapChain();
		void CreateGraphicPipeline();
		void CreateFrameBuffer();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, std::string pipeline_name);
		void CreateSyncObjects();
		void recreateSwapChain();
		void createVertexBuffer();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		bool isMinimized() const;




	};
}

