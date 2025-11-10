#pragma once
#include "VulkanGraphicResourceManager.h"
namespace vulkan {
	class VulkanGraphicResourceManager;

	class VulkanCommandBuffer : RHICommnadBuffer 
	{
	public:
		VulkanCommandBuffer();
		~VulkanCommandBuffer();
		void init(VulkanGraphicResourceManager* gpu_resource);
		void shutdown();

		VkCommandBuffer	_vk_command_buffer;
	private:
		VulkanGraphicResourceManager* gpu_resource;
		

	};

	class VulkanCommandBufferManager : RHICommandBufferManager
	{

	public:
		VulkanCommandBufferManager();
		~VulkanCommandBufferManager();
		void init(VulkanGraphicResourceManager* gpu_resource);
		void shutdown();
		VulkanCommandBuffer* GetCommandBuffer(uint32_t frame_index, uint32_t thread_index, bool begin);
	private:
		VulkanGraphicResourceManager* gpu_resource;

	};

}

