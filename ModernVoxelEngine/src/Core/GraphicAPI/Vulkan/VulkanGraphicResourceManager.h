#pragma once
#include "VulkanClasses.h"
#include "ResourceCreation.hpp"

VK_DEFINE_HANDLE(VmaAllocator)

namespace vulkan {
	class VulkanCommandBuffer;
	class VulkanCommandBufferManager;

	
	 class VulkanGraphicResourceManager final : public RHIGraphicResourceManager
	{
	public:
		VulkanGraphicResourceManager(Instance& instance, Device& device, SwapChain& swapchain, DescriptorPoolManager& descriptor_pool, 
			DescriptorLayoutManager& descriptor_layouts, GraphicPipelineManager& graphic_pipelines);
		~VulkanGraphicResourceManager();
		void											Init();
		void											Shutdown();

		VkDevice										VKDevice() { return _vk_device.Handle(); };

		BufferHandle									CreateBufferResouce(const BufferCreation& creation);
		TextureHandle									CreateTextureResource(const TextureCreation& creation);
		TextureHandle									CreateTextureViewResource(const TextureViewCreation& creation);
		SamplerHandle									CreateSampler(const SamplerCreation& creation);
		PipelineHandle									CreatePipeline(const PipelineCreation& creation, const char* cache_path = nullptr);
		DescriptorSetLayoutHandle						CreateDescriptorSetLayout(const  DescriptorSetLayoutCreation& creation);
		DescriptorSetHandle								CreateDescriptorSet(const DescriptorSetCreation& creation);
		RenderPassHandle								CreateRenderPass(const RenderPassCreation& creation);
		FramebufferHandle								CreateFrameBuffer(const FramebufferCreation& creation);
		ShaderStateHandle								CreateShaderState(const ShaderStateCreation& creation);

		VulkanBuffer*									AccessBuffer(BufferHandle handle);
		VulkanTexture*									AccessTexture(TextureHandle handle);
		
		VulkanCommandBuffer*							GetCommandBuffer(uint32_t thread_index, uint32_t frame_index, bool begin);

		void											SubmitCommandBuffer(QueueFamily queue, const VkSubmitInfo* submit_info_, uint32_t submit_count_, VkFence fence);;

		bool											IsSynchronizetion2ExtensionPresent() { return _vk_device.synchronization2_extension_present; };

		//Image utils
		void UtilAddImageBarrier(
			VkCommandBuffer command_buffer_,
			VkImage image_,
			ResourceState old_state_,
			ResourceState new_state_,
			uint32_t base_mip_level_,
			uint32_t mip_count_, bool is_depth_);
		PFN_vkCmdPipelineBarrier2KHR						vkCmdPipelineBarrier2KHR;

		

		
	public:
		utils::vector<ResourceUpdate>					_texture_bindless_updates;
		utils::vector<VmaAllocationInfo>				_debug_infos;
		VkAllocationCallbacks*							vulkan_allocation_callbacks;
		VmaAllocator									_vma_allocator;
		//current frame
		utils::vector<GpuThreadFramePools>				_thread_frame_pools;
		uint32_t										current_frame;
		bool											_is_bindless = false;
	private:
		Instance&									    _vk_instance;
		Device&											_vk_device;
		SwapChain&										_vk_swapchain;
		DescriptorPoolManager&							_vk_descriptor_pools;
		DescriptorLayoutManager&						_vk_descriptor_layouts;
		GraphicPipelineManager&							_vk_graphic_pipelines;

		ResourcePool<VulkanBuffer>						_buffers;
		ResourcePool<VulkanTexture>						_textures;
		ResourcePool<VulkanRenderPass>					_render_passes;
		ResourcePool<VulkanDesciptorSetLayout>			_descriptor_set_layouts;
		ResourcePool<VulkanPipeline>					_pipelines;
		ResourcePool<VulkanShaderState>					_shaders;
		ResourcePool<VulkanDesciptorSet>				_descriptor_sets;
		ResourcePool<VulkanSampler>						_samplers;
		
		// Primitive resources
		BufferHandle									_fullscrean_vertex_buffer;
		RenderPassHandle								_swapchain_pass;
		SamplerHandle									_default_sampler;

		//Dumy resources
		TextureHandle									_dummy_texture;
		BufferHandle									_dummy_constant_buffer;
		uint32_t										dynamic_max_per_frame_size;
		BufferHandle									dynamic_buffer;
		uint8_t*										dynamic_mapped_memory;
		uint32_t										dynamic_allocated_size;
		uint32_t										dynamic_per_frame_size;

		
	

	

		
	};
}


