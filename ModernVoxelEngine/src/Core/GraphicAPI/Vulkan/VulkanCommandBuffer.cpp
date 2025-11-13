#include "VulkanCommandBuffer.h"


namespace vulkan {


	VulkanCommandBuffer::VulkanCommandBuffer()
	{
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		shutdown();
	}
	static const uint32_t k_descriptor_sets_pool_size = 4096;

	void VulkanCommandBuffer::reset() {
		_is_recording = false;
		_current_render_pass = nullptr;
		_current_framebuffer = nullptr;
		_current_pipeline = nullptr;
		_current_command = 0;

		vkResetDescriptorPool(gpu_resource->VKDevice(), _vk_descriptor_pool, 0);
		//TODO: release local descriptor sets
		
	}
	void VulkanCommandBuffer::Begin()
	{
		if (!_is_recording) {
			VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(_vk_command_buffer, &beginInfo);
			_is_recording = true;
		}
	}
	void VulkanCommandBuffer::End()
	{
		if (_is_recording) {
			vkEndCommandBuffer(_vk_command_buffer);

			_is_recording = false;
		}
	}
	void VulkanCommandBuffer::init(VulkanGraphicResourceManager* gpu_resource_)
	{
		gpu_resource = gpu_resource_;
		//TODO::可以优化为动态创建，用新layout, 使用gpu resource中的descriptor pool和descriptor set layout manager
		static const uint32_t k_global_pool_elements = 128;
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, k_global_pool_elements },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, k_global_pool_elements}
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = k_descriptor_sets_pool_size;
		pool_info.poolSizeCount = (uint32_t)ArraySize(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		Check(vkCreateDescriptorPool(gpu_resource->VKDevice(), &pool_info, gpu_resource->vulkan_allocation_callbacks, &_vk_descriptor_pool), "Create Descriptor Pool for buffer");
		_descriptor_sets.init(k_descriptor_sets_pool_size);
		
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


	void VulkanCommandBufferManager::init(VulkanGraphicResourceManager* gpu_resource_, uint32_t num_threads_)
	{
		gpu_resource = gpu_resource_;
		_num_pools_per_frame = num_threads_;

		const uint32_t total_pools = _num_pools_per_frame * k_max_frames;

		_used_buffers.resize(total_pools);
		_used_seondary_command_buffers.resize(total_pools);

		for (uint32_t i = 0; i < total_pools; i++) {
			_used_buffers[i] = 0;
			_used_seondary_command_buffers[i] = 0;
		}

		const uint32_t total_buffers = total_pools * _num_command_buffers_per_thread;
		_command_buffers.resize(total_buffers);
		const uint32_t total_secondary_buffers = total_buffers * k_secondary_command_buffers_count;
		_secondary_command_buffers.resize(total_secondary_buffers);
		for (uint32_t i = 0; i < total_buffers; i++) {
			VkCommandBufferAllocateInfo cmd = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
			const uint32_t frame_index = i / (_num_command_buffers_per_thread * _num_pools_per_frame);
			const uint32_t thread_index = (i / _num_command_buffers_per_thread) % _num_pools_per_frame;
			const uint32_t pool_index = PoolFromIndices(frame_index, thread_index);
			cmd.commandPool = gpu_resource->_thread_frame_pools[pool_index].vulkan_command_pool;
			cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmd.commandBufferCount = 1;
			
			VulkanCommandBuffer& current_command_buffer = _command_buffers[i];
			vkAllocateCommandBuffers(gpu_resource->VKDevice(), &cmd, &current_command_buffer._vk_command_buffer);

			current_command_buffer._handle = i;
			current_command_buffer.thread_frame_pool = &gpu_resource->_thread_frame_pools[pool_index];
			current_command_buffer.init(gpu_resource);
			
		}
		uint32_t handle = total_buffers;
		for (uint32_t pool_index = 0; pool_index < total_pools; ++pool_index) {
			VkCommandBufferAllocateInfo cmd = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
			cmd.commandPool = gpu_resource->_thread_frame_pools[pool_index].vulkan_command_pool;
			cmd.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			cmd.commandBufferCount = k_secondary_command_buffers_count;
			VkCommandBuffer secondary_buffers[k_secondary_command_buffers_count];
			vkAllocateCommandBuffers(gpu_resource->VKDevice(), &cmd, secondary_buffers);
			for (uint32_t scb_index = 0; scb_index < k_secondary_command_buffers_count; ++scb_index) {
				VulkanCommandBuffer cb{};
				cb._vk_command_buffer = secondary_buffers[scb_index];
				cb._handle = handle++;
				cb.thread_frame_pool = &gpu_resource->_thread_frame_pools[pool_index];
				cb.init(gpu_resource);
				_secondary_command_buffers.push_back(cb);
			}
		} 
	}



	void VulkanCommandBufferManager::shutdown()
	{
	}

	VulkanCommandBuffer* VulkanCommandBufferManager::GetCommandBuffer(uint32_t frame_index, uint32_t thread_index, bool begin)
	{
		const uint32_t pool_index = PoolFromIndices(frame_index, thread_index);
		uint32_t current_used_buffer = _used_buffers[pool_index];
		assert(current_used_buffer < _num_command_buffers_per_thread);
		if (begin) {
			_used_buffers[pool_index] = current_used_buffer + 1;
		}
		VulkanCommandBuffer* cb = &_command_buffers[(pool_index * _num_command_buffers_per_thread) + current_used_buffer];
		if (begin) {
			cb->reset();
			cb->Begin();
		}
		return cb;
	}
	VulkanCommandBuffer* VulkanCommandBufferManager::GetSecondareyCommandBuffer(uint32_t frame, uint32_t thread_index)
	{
		return nullptr;
	}
	uint32_t VulkanCommandBufferManager::PoolFromIndices(uint32_t frame_index_, uint32_t thread_index_)
	{
		return (frame_index_ * _num_pools_per_frame) + thread_index_;
	}
}