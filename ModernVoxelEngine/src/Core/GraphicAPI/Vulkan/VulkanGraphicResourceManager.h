#pragma once
#include "VulkanClasses.h"

namespace vulkan {
	class VulkanGraphicResourceManager : RHIGraphicResourceManager
	{
	public:
		VulkanGraphicResourceManager(Instance& instance, Device& device, SwapChain& swapchain, DescriptorPoolManager& descriptor_pool, 
			DescriptorLayoutManager& descriptor_layouts, GraphicPipelineManager& graphic_pipelines);
		~VulkanGraphicResourceManager();
		void											Init();
		void											Shutdown();

		BufferHandle									CreateBufferResouce(const BufferCreation& creation);
		VulkanBuffer*									AccessBuffer(BufferHandle handle);
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

		ResourcePool<Vertex1>							_vertexs;

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
		VmaAllocator									_vma_allocator;
	};
}


