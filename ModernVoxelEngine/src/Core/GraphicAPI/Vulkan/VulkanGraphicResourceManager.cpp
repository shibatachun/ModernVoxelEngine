#include "VulkanGraphicResourceManager.h"
namespace vulkan {
	VulkanGraphicResourceManager::VulkanGraphicResourceManager(
		Instance& instance,
		Device& device,
		SwapChain& swapchain,
		DescriptorPoolManager& descriptor_pool,
		DescriptorLayoutManager& descriptor_layouts,
		GraphicPipelineManager& graphic_pipelines) : _vk_instance(instance), _vk_device(device), _vk_swapchain(swapchain), _vk_descriptor_pools(descriptor_pool)
		, _vk_descriptor_layouts(descriptor_layouts), _vk_graphic_pipelines(graphic_pipelines)
	{
	}
	VulkanGraphicResourceManager::~VulkanGraphicResourceManager()
	{

	}
	void vulkan::VulkanGraphicResourceManager::Init() {
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = _vk_device.PhysicalDevice();
		allocatorInfo.device = _vk_device.Handle();
		allocatorInfo.instance = _vk_instance.Handle();

		_buffers.init(1024);

		Check(vmaCreateAllocator(&allocatorInfo, &_vma_allocator), "Create Vma allocator");
	}
	void vulkan::VulkanGraphicResourceManager::Shutdown() {

	}
	BufferHandle vulkan::VulkanGraphicResourceManager::CreateBufferResouce(const BufferCreation& creation)
	{
		BufferHandle handle = { _buffers.obtain_resouce()};
		if (handle.index == invalidIndex) {
			return handle;
		}

		VulkanBuffer* buffer = _buffers.get(handle.index);
		buffer->name = creation.name;
		buffer->_size = creation.size;
		buffer->_typeFlags = creation.type_flags;
		buffer->_usage = creation.usage;
		buffer->_handle = handle;
		buffer->_global_offset = 0;
		buffer->_parentBuffer = Invalid_Buffer;

		static const VkBufferUsageFlags k_dynamic_buffer_mask = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		const bool use_global_buffer = (creation.type_flags & k_dynamic_buffer_mask) != 0;
		if (creation.usage == ResourceUsageType::Dynamic && use_global_buffer) {
			//TODO: create dynamic buffer
		}
		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | creation.type_flags;
		buffer_info.size = creation.size > 0 ? creation.size : 1;       // 0 sized creations are not permitted.

		VmaAllocationCreateInfo memory_info{};
		memory_info.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
		memory_info.usage = VMA_MEMORY_USAGE_AUTO;

		VmaAllocationInfo allocation_info{};
		Check(vmaCreateBuffer(_vma_allocator, &buffer_info, &memory_info, &buffer->_vkBuffer, &buffer->_vmaAllocation, &allocation_info), "Create buffer");
		buffer->_vkDeviceMemory = allocation_info.deviceMemory;
		if (creation.initial_data) {
			void* data;
			vmaMapMemory(_vma_allocator, buffer->_vmaAllocation, &data);
			memcpy(data, creation.initial_data, (size_t)creation.size);
			vmaUnmapMemory(_vma_allocator, buffer->_vmaAllocation);
		}

		return BufferHandle();
	}
}