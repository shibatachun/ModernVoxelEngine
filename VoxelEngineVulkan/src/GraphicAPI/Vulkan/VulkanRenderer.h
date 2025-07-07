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

	private:
		const VkPresentModeKHR _presentMode;
		GLFWwindow* _window;
		asset::AssetManager& _assetManager;
		std::unique_ptr<class vulkan::Instance> _instance;
		std::unique_ptr<class vulkan::Surface> _surface;
		std::unique_ptr<class vulkan::DebugUtilsMessenger> _debugMessenger;
		std::unique_ptr<class vulkan::Device> _devices;
		std::unique_ptr<class vulkan::SwapChain> _swapchain;
		std::unique_ptr<class vulkan::GraphicPipeline> _graphicsPipline;
		std::unique_ptr<class vulkan::RenderPass> _renderPass;

		std::vector<VkFramebuffer> _swapChainFramebuffers;
		VkCommandPool _commandPool;
		VkCommandBuffer _commandBuffer;
		VkSemaphore _imageAvailableSemaphore;
		VkSemaphore _renderFinishedSemaphore;
		VkFence _inFlightFence;
		
		
		
	private:
		void SetPhysicalDevices();
		void SetSwapChain();
		void CreateGraphicPipeline();
		void CreateFrameBuffer();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, std::string pipeline_name);
		void CreateSyncObjects();
		bool isMinimized() const;


	};
}

