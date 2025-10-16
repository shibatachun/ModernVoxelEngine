#pragma once
#include "VulkanCommon.h"
namespace vulkan {

	static const uint32_t					invalidIndex = 0xffffffff;

	typedef uint32_t                         ResourceHandle;


	static const uint8_t                     k_max_image_outputs = 8;                // Maximum number of images/render_targets/fbo attachments usable.
	static const uint8_t                     k_max_descriptor_set_layouts = 8;       // Maximum number of layouts in the pipeline.
	static const uint8_t                     k_max_shader_stages = 5;                // Maximum simultaneous shader stages. Applicable to all different type of pipelines.
	static const uint8_t                     k_max_descriptors_per_set = 16;         // Maximum list elements for both descriptor set layout and descriptor sets.
	static const uint8_t                     k_max_vertex_streams = 16;
	static const uint8_t                     k_max_vertex_attributes = 16;

	static const uint32_t                    k_submit_header_sentinel = 0xfefeb7ba;
	static const uint32_t                    k_max_resource_deletions = 64;


	struct BufferHandle {
		ResourceHandle                  index;
	}; // struct BufferHandle

	struct TextureHandle {
		ResourceHandle                  index;
	}; // struct TextureHandle

	struct ShaderStateHandle {
		ResourceHandle                  index;
	}; // struct ShaderStateHandle

	struct SamplerHandle {
		ResourceHandle                  index;
	}; // struct SamplerHandle

	struct DescriptorSetLayoutHandle {
		ResourceHandle                  index;
	}; // struct DescriptorSetLayoutHandle

	struct DescriptorSetHandle {
		ResourceHandle                  index;
	}; // struct DescriptorSetHandle

	struct PipelineHandle {
		ResourceHandle                  index;
	}; // struct PipelineHandle

	struct RenderPassHandle {
		ResourceHandle                  index;
	};
	namespace ResourceUsageType {
		enum Enum {
			Immutable, Dynamic, Stream, Count
		};

		enum Mask {
			Immutable_mask = 1 << 0, Dynamic_mask = 1 << 1, Stream_mask = 1 << 2, Count_mask = 1 << 3
		};

		static const char* _sValueNames[] = {
			"Immutable", "Dynamic", "Stream", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? _sValueNames[(int)e] : "unsupported");
		}
	}
	namespace ColorWriteEnabled {
		enum Enum {
			Red, Green, Blue, Alpha, All, Count
		};

		enum Mask {
			Red_mask = 1 << 0, Green_mask = 1 << 1, Blue_mask = 1 << 2, Alpha_mask = 1 << 3, All_mask = Red_mask | Green_mask | Blue_mask | Alpha_mask
		};

		static const char* s_value_names[] = {
			"Red", "Green", "Blue", "Alpha", "All", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace ColorWriteEnabled

	namespace TextureType {
		enum Enum {
			Texture1D, Texture2D, Texture3D, Texture_1D_Array, Texture_2D_Array, Texture_Cube_Array, Count
		};

		enum Mask {
			Texture1D_mask = 1 << 0, Texture2D_mask = 1 << 1, Texture3D_mask = 1 << 2, Texture_1D_Array_mask = 1 << 3, Texture_2D_Array_mask = 1 << 4, Texture_Cube_Array_mask = 1 << 5, Count_mask = 1 << 6
		};

		static const char* s_value_names[] = {
			"Texture1D", "Texture2D", "Texture3D", "Texture_1D_Array", "Texture_2D_Array", "Texture_Cube_Array", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace TextureType
	namespace FillMode {
		enum Enum {
			Wireframe, Solid, Point, Count
		};

		enum Mask {
			Wireframe_mask = 1 << 0, Solid_mask = 1 << 1, Point_mask = 1 << 2, Count_mask = 1 << 3
		};

		static const char* s_value_names[] = {
			"Wireframe", "Solid", "Point", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace FillMode

	namespace RenderPassType {

		enum Enum {
			Geometry, Swapchain, Compute
		};
	} // namespace RenderPassType

	struct StencilOperationState {

		VkStencilOp                     fail = VK_STENCIL_OP_KEEP;
		VkStencilOp                     pass = VK_STENCIL_OP_KEEP;
		VkStencilOp                     depth_fail = VK_STENCIL_OP_KEEP;
		VkCompareOp                     compare = VK_COMPARE_OP_ALWAYS;
		uint32_t                             compare_mask = 0xff;
		uint32_t                             write_mask = 0xff;
		uint32_t                             reference = 0xff;

	}; // struct StencilOperationState
	struct DepthStencilCreation {

		StencilOperationState           front;
		StencilOperationState           back;
		VkCompareOp                     depth_comparison = VK_COMPARE_OP_ALWAYS;

		uint8_t                              depth_enable : 1;
		uint8_t                              depth_write_enable : 1;
		uint8_t                              stencil_enable : 1;
		uint8_t                              pad : 5;

		// Default constructor
		DepthStencilCreation() : depth_enable(0), depth_write_enable(0), stencil_enable(0) {
		}

		DepthStencilCreation& set_depth(bool write, VkCompareOp comparison_test);

	}; // struct DepthStencilCreation

	struct BlendState {

		VkBlendFactor                   source_color = VK_BLEND_FACTOR_ONE;
		VkBlendFactor                   destination_color = VK_BLEND_FACTOR_ONE;
		VkBlendOp                       color_operation = VK_BLEND_OP_ADD;

		VkBlendFactor                   source_alpha = VK_BLEND_FACTOR_ONE;
		VkBlendFactor                   destination_alpha = VK_BLEND_FACTOR_ONE;
		VkBlendOp                       alpha_operation = VK_BLEND_OP_ADD;

		ColorWriteEnabled::Mask         color_write_mask = ColorWriteEnabled::All_mask;

		uint8_t                              blend_enabled : 1;
		uint8_t                              separate_blend : 1;
		uint8_t                              pad : 6;


		BlendState() : blend_enabled(0), separate_blend(0) {
		}

		BlendState& set_color(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
		BlendState& set_alpha(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
		BlendState& set_color_write_mask(ColorWriteEnabled::Mask value);

	}; // struct BlendState

	struct BlendStateCreation {

		BlendState                      blend_states[k_max_image_outputs];
		uint32_t                             active_states = 0;

		BlendStateCreation& reset();
		BlendState& add_blend_state();

	}; // BlendStateCreation

	struct RasterizationCreation {

		VkCullModeFlagBits              cull_mode = VK_CULL_MODE_NONE;
		VkFrontFace                     front = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		FillMode::Enum                  fill = FillMode::Solid;
	}; // struct RasterizationCreation

	struct VulkanBuffer {
		VkBuffer				_vkBuffer;
		VmaAllocation			_vmaAllocation;
		VkDeviceMemory			_vkDeviceMemory;
		VkDeviceSize			_vkDeviceSize;

		VkBufferUsageFlags		_typeFlags = 0;
		ResourceUsageType::Enum _usage = ResourceUsageType::Immutable;

		uint32_t				_size = 0;
		uint32_t				_global_offset = 0;

		BufferHandle			_handle;
		BufferHandle			_parentBuffer;

		const char* _name = nullptr;

	};

	struct VulkanSampler {

		VkSampler                       _vkSampler;

		VkFilter                        _minFilter = VK_FILTER_NEAREST;
		VkFilter                        _magFilter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             _mipFilter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		const char* name = nullptr;

	};

	struct VulkanTexture {

		VkImage                         image;
		VkImageView                     view;
		VkFormat                        vk_format;
		VkImageLayout                   vk_image_layout;
		VmaAllocation                   vma_allocation;

		uint16_t                        width = 1;
		uint16_t                        height = 1;
		uint16_t                        depth = 1;
		uint8_t                         mipmaps = 1;
		uint8_t                         flags = 0;

		TextureHandle                   handle;
		TextureType::Enum               type = TextureType::Texture2D;

		VulkanSampler* sampler = nullptr;

		const char* name = nullptr;
	}; // struct TextureVulkan

	struct VulkanShaderState {

		VkPipelineShaderStageCreateInfo shader_stage_info[k_max_shader_stages];

		const char* name = nullptr;

		uint32_t                             active_shaders = 0;
		bool                            graphics_pipeline = false;


	}; // struct ShaderStateVulkan

	struct VulkanDescriptorBinding {

		VkDescriptorType					 type;
		uint16_t                             start = 0;
		uint16_t                             count = 0;
		uint16_t                             set = 0;

		const char* name = nullptr;
	}; // struct ResourceBindingVulkan

	struct VulkanDesciptorSetLayout {

		VkDescriptorSetLayout					vk_descriptor_set_layout;

		VkDescriptorSetLayoutBinding* vk_binding = nullptr;
		VulkanDescriptorBinding* bindings = nullptr;
		uint16_t								num_bindings = 0;
		uint16_t								set_index = 0;

		DescriptorSetLayoutHandle				handle;

	}; // struct DesciptorSetLayoutVulkan


	struct VulkanDesciptorSet {

		VkDescriptorSet						vk_descriptor_set;

		ResourceHandle* resources = nullptr;
		SamplerHandle* samplers = nullptr;
		uint16_t* bindings = nullptr;

		const VulkanDesciptorSetLayout* layout = nullptr;
		uint32_t                             num_resources = 0;
	}; // struct DesciptorSetVulkan


	struct VulkanPipeline {

		VkPipeline                      vk_pipeline;
		VkPipelineLayout                vk_pipeline_layout;

		VkPipelineBindPoint             vk_bind_point;

		ShaderStateHandle               shader_state;

		const VulkanDesciptorSetLayout* descriptor_set[k_max_descriptor_set_layouts];
		DescriptorSetLayoutHandle       descriptor_set_layout_handle[k_max_descriptor_set_layouts];
		uint32_t                        num_active_layouts = 0;

		DepthStencilCreation            depth_stencil;
		BlendStateCreation              blend_state;
		RasterizationCreation           rasterization;

		PipelineHandle                  handle;
		bool                            graphics_pipeline = true;

	}; // struct PipelineVulkan

	struct VulkanRenderPass {

		VkRenderPass                    vk_render_pass;
		VkFramebuffer                   vk_frame_buffer;


		TextureHandle                   output_textures[k_max_image_outputs];
		TextureHandle                   output_depth;

		RenderPassType::Enum            type;

		float                             scale_x = 1.f;
		float                             scale_y = 1.f;
		uint16_t                             width = 0;
		uint16_t                             height = 0;
		uint16_t                             dispatch_x = 0;
		uint16_t                             dispatch_y = 0;
		uint16_t                             dispatch_z = 0;

		uint8_t                              resize = 0;
		uint8_t                              num_render_targets = 0;

		const char* name = nullptr;
	}; // struct RenderPass
	struct VulkanFramebuffer {

		// NOTE(marco): this will be a null handle if dynamic rendering is available
		VkFramebuffer                   vk_framebuffer;

		// NOTE(marco): cache render pass handle
		RenderPassHandle                render_pass;

		uint16_t                             width = 0;
		uint16_t                             height = 0;

		float                             scale_x = 1.f;
		float                             scale_y = 1.f;

		TextureHandle						color_attachments[k_max_image_outputs];
		TextureHandle						depth_stencil_attachment;
		uint32_t                             num_color_attachments;

		uint8_t                              resize = 0;

		const char* name = nullptr;
	}; // struct Framebuffer

	template <typename T>
	struct ResourcePool {
		void init(uint32_t pool_size) {
			resources.resize(pool_size);
			freeIndices.resize(pool_size);
			usedIndices.resize(poolSize);
			

			for (uint32_t i = 0; i < pool_size; ++i) {
				freeIndices[i] = i;
			}
			freeIndicesHead = 0;
			usedIndices = 0;
		}
		void shutdown() {

		}

		T* obtain() {
			uint32_t resourceIndex = obtain_resouce();
			if (resourceIndex != invalidIndex) {
				T* resource = get(resourceIndex);
				resource->poolIndex = resourceIndex;
				return resource;
			}
		}
		void release(T* resource) {

		}

		T* get(uint32_t index) {

		}
		const T* get(uint32_t index) const {

		}

	private:
		uint32_t poolSize;
		std::vector<T> resources;
		std::vector<uint32_t> freeIndices;
		uint32_t usedIndices;
		uint32_t freeIndicesHead;
	private:
		uint32_t obtain_resouce() {
			if (freeIndicesHead < poolSize) {
				const uint32_t freeIndex = freeIndices[freeIndicesHead++];
				--usedIndices;
				
				return freeIndex;
			}
			return invalidIndex;
		}
		void release_resource(uint32_t handle) {
			freeIndices[--freeIndicesHead] = handle;
			--usedIndices;
		}
		void free_all_resource() {
			freeIndicesHead = 0;
			usedIndices = 0;
			for (uint32_t i = 0; i < poolSize; i++) {
				freeIndices[i] = i;
			}
		}

		void* access_resource(uint32_t handle) {
			if (handle != invalidIndex) {
				return &resources[handle];
			}
			return nullptr;
		}
		const void* access_resource(uint32_t handle) const {
			if (handle != invalidIndex) {
				return &resources[handle];
			}
			return nullptr;
		}
	


	};
}

