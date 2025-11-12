#pragma once
#include "VulkanGraphicResourceManager.h"
namespace vulkan {
	 
	static const uint32_t k_secondary_command_buffers_count = 3;
	class VulkanCommandBuffer : RHICommnadBuffer 
	{
	public:
		VulkanCommandBuffer();
		~VulkanCommandBuffer();
		void init(VulkanGraphicResourceManager* gpu_resource);
		void shutdown();
		void reset();

		void Begin();
		void End();


		
	public:
		
		GpuThreadFramePools* thread_frame_pool;
		VkCommandBuffer	_vk_command_buffer;
		bool _is_recording;
		VulkanRenderPass* _current_render_pass;
		VulkanFramebuffer* _current_framebuffer;
		VulkanPipeline* _current_pipeline;
		VkClearValue	_clear_values[k_max_image_outputs + 1];
		uint32_t _current_command;
		uint32_t _handle;
		ResourceHandle	_resource_handle;

		
	private:
		VulkanGraphicResourceManager* gpu_resource;
		VkDescriptorPool _vk_descriptor_pool;
		ResourcePool<VulkanDesciptorSet>	_descriptor_sets;

		


		

	};

	class VulkanCommandBufferManager : RHICommandBufferManager
	{

	public:
		VulkanCommandBufferManager();
		~VulkanCommandBufferManager();
		void init(VulkanGraphicResourceManager* gpu_resource, uint32_t num_threads_);
		void shutdown();
		VulkanCommandBuffer* GetCommandBuffer(uint32_t frame_index, uint32_t thread_index, bool begin);
		VulkanCommandBuffer* GetSecondareyCommandBuffer(uint32_t frame, uint32_t thread_index);

		uint16_t PoolFromIndex(uint32_t index) {
			return (uint16_t)index / _num_pools_per_frame;
		}
		uint32_t PoolFromIndices(uint32_t frame_index_, uint32_t thread_index_);
		

	private:
		VulkanGraphicResourceManager* gpu_resource;
		uint32_t _num_pools_per_frame;
		uint32_t _num_command_buffers_per_thread = 3;
		utils::vector<VulkanCommandBuffer> _command_buffers;
		utils::vector<VulkanCommandBuffer> _secondary_command_buffers;
		utils::vector<uint8_t> _used_buffers;
		utils::vector<uint8_t> _used_seondary_command_buffers;

	};

}

