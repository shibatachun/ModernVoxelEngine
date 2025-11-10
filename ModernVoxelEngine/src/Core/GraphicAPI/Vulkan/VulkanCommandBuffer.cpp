#include "VulkanCommandBuffer.h"


namespace vulkan {


	VulkanCommandBuffer::VulkanCommandBuffer()
	{
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		shutdown();
	}

	void VulkanCommandBuffer::init(VulkanGraphicResourceManager* gpu_resource_)
	{
		gpu_resource = gpu_resource_;
	}



	void VulkanCommandBuffer::shutdown()
	{
	}



	VulkanCommandBufferManager::VulkanCommandBufferManager()
	{
	}

	VulkanCommandBufferManager::~VulkanCommandBufferManager()
	{
	}


	void VulkanCommandBufferManager::init(VulkanGraphicResourceManager* gpu_resource_)
	{
		gpu_resource = gpu_resource_;
	}



	void VulkanCommandBufferManager::shutdown()
	{
	}

	VulkanCommandBuffer* GetCommandBuffer(uint32_t frame_index, uint32_t thread_index, bool begin)
	{

	}
}