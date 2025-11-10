#include "VulkanGraphicResourceManager.h"
#include "VulkanCommandBuffer.h"

#include <vma/vk_mem_alloc.h>



namespace vulkan {

	static VulkanCommandBufferManager m_command_buffer;

	VulkanGraphicResourceManager::VulkanGraphicResourceManager(
		Instance& instance,
		Device& device,
		SwapChain& swapchain,
		DescriptorPoolManager& descriptor_pool,
		DescriptorLayoutManager& descriptor_layouts,
		GraphicPipelineManager& graphic_pipelines) : _vk_instance(instance), _vk_device(device), _vk_swapchain(swapchain), _vk_descriptor_pools(descriptor_pool)
		, _vk_descriptor_layouts(descriptor_layouts), _vk_graphic_pipelines(graphic_pipelines)
	{
		_is_bindless = device.isBindless;
	}
	VulkanGraphicResourceManager::~VulkanGraphicResourceManager()
	{

	}
	static void vulkan_create_texture_view(VulkanGraphicResourceManager& resouce_manager, const TextureViewCreation& creation, VulkanTexture* texture) {
		VkImageViewCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		info.image = texture->vk_image;
		info.format = texture->vk_format;

		if (TextureFormatted::has_depth_or_stencil(texture->vk_format)) {
			info.subresourceRange.aspectMask = TextureFormatted::has_depth(texture->vk_format) ? VK_IMAGE_ASPECT_DEPTH_BIT : 0;
		}
		else {
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		info.viewType = creation._view_type;
		info.subresourceRange.baseMipLevel = creation._sub_resource.mip_level_count;
		info.subresourceRange.baseArrayLayer = creation._sub_resource.array_base_layer;
		info.subresourceRange.layerCount = creation._sub_resource.array_layer_count;
		info.subresourceRange.levelCount = creation._sub_resource.mip_level_count;
		Check(vkCreateImageView(resouce_manager.VKDevice(), &info, resouce_manager.vulkan_allocation_callbacks, &texture->vk_image_view), "Create texture " + std::string(texture->name));

	}

	static void VulkanCreateTexture(VulkanGraphicResourceManager& resource_manager, const TextureCreation& creation, TextureHandle handle, VulkanTexture* texture) {
		//判断要创建的texture 类型
		bool is_cubemap = false;
		uint32_t layerCount = creation._array_layer_count;
		if (creation._type == TextureType::TextureCube || creation._type == TextureType::Texture_Cube_Array) {
			is_cubemap = true;
		}
		const bool is_sparse_texture = (creation._flags & TextureFlags::Sparse_mask) == TextureFlags::Sparse_mask;

		texture->width = creation._width;
		texture->height = creation._height;
		texture->depth = creation._depth;
		texture->mip_base_level = 0;
		texture->array_base_layer = 0;
		texture->array_layer_count = layerCount;
		texture->mip_level_count = creation._mip_level_count;
		texture->type = creation._type;
		texture->name = creation._name;
		texture->vk_usage = vulkan_get_image_usage(creation);
		texture->vk_format = creation._format;
		texture->sampler = nullptr;
		texture->flags = creation._flags;
		texture->parent_txture = Invalid_Texture;
		texture->handle = handle;
		texture->sparse = is_sparse_texture;
		texture->alias_texture = Invalid_Texture;

		//开始创建流程，根据creation info创建vkimage
		VkImageCreateInfo	image_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		image_info.format = texture->vk_format;
		image_info.flags = (is_cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0) | (is_sparse_texture ? (VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT) : 0);
		image_info.imageType = to_vk_image_type(texture->type);
		image_info.extent.width = creation._width;
		image_info.extent.height = creation._height;
		image_info.extent.depth = creation._depth;
		image_info.mipLevels = creation._mip_level_count;
		image_info.arrayLayers = layerCount;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = texture->vk_usage;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo memory_info{};
		memory_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (creation._alias.index == Invalid_Texture.index) {
			if (is_sparse_texture) {
				Check(vkCreateImage(resource_manager.VKDevice(), &image_info, resource_manager.vulkan_allocation_callbacks, &texture->vk_image), creation._name);

			}
			else {
				Check(vmaCreateImage(resource_manager._vma_allocator, &image_info, &memory_info, &texture->vk_image, &texture->vma_allocation, nullptr), creation._name);
			}
		}
		else {
			VulkanTexture* alias_textures = resource_manager.AccessTexture(creation._alias);
			assert(alias_textures != nullptr);
			assert(!is_sparse_texture);

			texture->vma_allocation = 0;
			Check(vmaCreateAliasingImage(resource_manager._vma_allocator, alias_textures->vma_allocation, &image_info, &texture->vk_image), creation._name);
			texture->alias_texture = creation._alias;
		}

		TextureViewCreation tvc;
		tvc.set_mips(0, creation._mip_level_count).set_array(0, layerCount).set_name(creation._name).set_view_type(to_vk_image_view_type(creation._type));

		vulkan_create_texture_view(resource_manager, tvc, texture);
		texture->state = RESOURCE_STATE_UNDEFINED;

		//TODO::Bindless support
		if (resource_manager._is_bindless) {
			ResourceUpdate	resource_update{ ResourceUpdateType::Texture, texture->handle.index, resource_manager.current_frame,0 };
			resource_manager._texture_bindless_updates.push_back(resource_update);
		}


	}

	static void UploadTextureData(VulkanTexture* texture_, void* upload_data_, VulkanGraphicResourceManager& resource_manager) {
		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		uint32_t image_size = texture_->width * texture_->height * 4;
		buffer_info.size = image_size;

		VmaAllocationCreateInfo memory_info{};
		memory_info.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
		memory_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VmaAllocationInfo allocation_info{};
		VkBuffer staging_buffer;
		VmaAllocation staging_allocation;
		Check(vmaCreateBuffer(resource_manager._vma_allocator, &buffer_info, &memory_info, &staging_buffer, &staging_allocation, &allocation_info), "Create Staging buffer for" + std::string(texture_->name));

		void* destination_data;
		vmaMapMemory(resource_manager._vma_allocator, staging_allocation, &destination_data);
		memcpy(destination_data, upload_data_, static_cast<size_t>(image_size));
		vmaUnmapMemory(resource_manager._vma_allocator, staging_allocation);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VulkanCommandBuffer* command_buffer = resource_manager.GetCommandBuffer(0, resource_manager.current_frame, false); 
		vkBeginCommandBuffer(command_buffer->_vk_command_buffer, &beginInfo);

	}
	void vulkan::VulkanGraphicResourceManager::Init() {
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = _vk_device.PhysicalDevice();
		allocatorInfo.device = _vk_device.Handle();
		allocatorInfo.instance = _vk_instance.Handle();
		
		_samplers.init(203);
		_buffers.init(1024);

		m_command_buffer.init(this);
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
		//buffer->name = creation.name;
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
		Check(vmaCreateBuffer(_vma_allocator, &buffer_info, &memory_info, &buffer->_vkBuffer, &buffer->_vmaAllocation, &allocation_info), "Create buffer" + std::string(creation._name));
		_debug_infos.push_back(allocation_info);
		buffer->_vkDeviceMemory = allocation_info.deviceMemory;
		if (creation.initial_data) {
			void* data;
			vmaMapMemory(_vma_allocator, buffer->_vmaAllocation, &data);
			memcpy(data, creation.initial_data, (size_t)creation.size);
			vmaUnmapMemory(_vma_allocator, buffer->_vmaAllocation);
		}

		return handle;
	}

	
	TextureHandle VulkanGraphicResourceManager::CreateTextureResource(const TextureCreation& creation)
	{
		uint32_t resource_index = _textures.obtain_resouce();
		TextureHandle handle = { resource_index };
		if (resource_index == invalidIndex) {
			return handle;
		}

		VulkanTexture* texture = AccessTexture(handle);

		VulkanCreateTexture(*this, creation, handle, texture);

		if (creation._initial_data) {

		}

		return TextureHandle();
	}
	TextureHandle VulkanGraphicResourceManager::CreateTextureViewResource(const TextureViewCreation& creation)
	{
		return TextureHandle();
	}
	PipelineHandle VulkanGraphicResourceManager::CreatePipeline(const PipelineCreation& creation, const char* cache_path)
	{
		return PipelineHandle();
	}
	SamplerHandle VulkanGraphicResourceManager::CreateSampler(const SamplerCreation& creation)
	{
		return SamplerHandle();
	}
	DescriptorSetLayoutHandle VulkanGraphicResourceManager::CreateDescriptorSetLayout(const DescriptorSetLayoutCreation& creation)
	{
		return DescriptorSetLayoutHandle();
	}
	DescriptorSetHandle VulkanGraphicResourceManager::CreateDescriptorSet(const DescriptorSetCreation& creation)
	{
		return DescriptorSetHandle();
	}
	RenderPassHandle VulkanGraphicResourceManager::CreateRenderPass(const RenderPassCreation& creation)
	{
		return RenderPassHandle();
	}
	FramebufferHandle VulkanGraphicResourceManager::CreateFrameBuffer(const FramebufferCreation& creation)
	{
		return FramebufferHandle();
	}
	ShaderStateHandle VulkanGraphicResourceManager::CreateShaderState(const ShaderStateCreation& creation)
	{
		return ShaderStateHandle();
	}
	VulkanBuffer* VulkanGraphicResourceManager::AccessBuffer(BufferHandle handle)
	{
		return _buffers.get(handle.index);
	}
	VulkanTexture* VulkanGraphicResourceManager::AccessTexture(TextureHandle handle)
	{
		return _textures.get(handle.index);
	}

	VulkanCommandBuffer* VulkanGraphicResourceManager::GetCommandBuffer(uint32_t thread_index, uint32_t frame_index, bool begin)
	{
		VulkanCommandBuffer* cb = m_command_buffer.GetCommandBuffer(thread_index, frame_index, begin);
	}



}