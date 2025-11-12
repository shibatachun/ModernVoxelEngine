#pragma once
#include "VulkanCommon.h"

#include <vma/vk_mem_alloc.h>

namespace vulkan {
	////////////////////////////////////////////////Enum/////////////////////////////////////
	typedef enum ResourceState {
		RESOURCE_STATE_UNDEFINED = 0,
		RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
		RESOURCE_STATE_INDEX_BUFFER = 0x2,
		RESOURCE_STATE_RENDER_TARGET = 0x4,
		RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
		RESOURCE_STATE_DEPTH_WRITE = 0x10,
		RESOURCE_STATE_DEPTH_READ = 0x20,
		RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
		RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
		RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
		RESOURCE_STATE_STREAM_OUT = 0x100,
		RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
		RESOURCE_STATE_COPY_DEST = 0x400,
		RESOURCE_STATE_COPY_SOURCE = 0x800,
		RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
		RESOURCE_STATE_PRESENT = 0x1000,
		RESOURCE_STATE_COMMON = 0x2000,
		RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
		RESOURCE_STATE_SHADING_RATE_SOURCE = 0x8000,
	} ResourceState;

	
	/// //////////////////////////////////////VulkanResouce////////////////////////////////////////////////////


	struct Resource {

		void				add_reference() { ++references; }
		void				remove_reference() { assert(references != 0); --references; }
		uint32_t			poolIndex;

		uint64_t            references = 0;
		const char*			name = nullptr;
		

	}; // struct Resource

	struct BlendState : Resource {

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


	struct Rect2D {
		float                             x = 0.0f;
		float                             y = 0.0f;
		float                             width = 0.0f;
		float                             height = 0.0f;
	}; // struct Rect2D

	//
	//
	struct Rect2DInt {
		int16_t                             x = 0;
		int16_t                             y = 0;
		uint16_t                             width = 0;
		uint16_t                             height = 0;
	}; // struct Rect2D

	//
	//
	struct Viewport {
		Rect2DInt                       rect;
		float                             min_depth = 0.0f;
		float                             max_depth = 0.0f;
	}; // struct Viewport

	//
	//
	struct ViewportState {
		uint32_t                             num_viewports = 0;
		uint32_t                             num_scissors = 0;

		Viewport* viewport = nullptr;
		Rect2DInt* scissors = nullptr;
	}; // struct ViewportState

	//
	//
	struct StencilOperationState {

		VkStencilOp                     fail = VK_STENCIL_OP_KEEP;
		VkStencilOp                     pass = VK_STENCIL_OP_KEEP;
		VkStencilOp                     depth_fail = VK_STENCIL_OP_KEEP;
		VkCompareOp                     compare = VK_COMPARE_OP_ALWAYS;
		uint32_t                             compare_mask = 0xff;
		uint32_t                             write_mask = 0xff;
		uint32_t                             reference = 0xff;

	}; // struct StencilOperationState

	//
	//
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


	struct BlendStateCreation {

		BlendState                      blend_states[k_max_image_outputs];
		uint32_t                             active_states = 0;

		BlendStateCreation& reset();
		BlendState& add_blend_state();

	}; // BlendStateCreation

	//
	//
	struct RasterizationCreation {

		VkCullModeFlagBits              cull_mode = VK_CULL_MODE_NONE;
		VkFrontFace                     front = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		FillMode::Enum                  fill = FillMode::Solid;
	}; // struct RasterizationCreation

	//
	//
	struct BufferCreation {

		VkBufferUsageFlags              type_flags = 0;
		ResourceUsageType::Enum         usage = ResourceUsageType::Immutable;
		uint32_t                             size = 0;
		uint32_t                              persistent = 0;
		uint32_t                              device_only = 0;
		void* initial_data = nullptr;

		const char* _name = nullptr;

		BufferCreation& reset();
		BufferCreation& set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage_, uint32_t size_);
		BufferCreation& set_data(void* data_);
		BufferCreation& set_name(const char* name_);
		BufferCreation& set_persistent(bool value);
		BufferCreation& set_device_only(bool value);

	}; // struct BufferCreation

	//
	//
	struct TextureCreation {

		void* _initial_data = nullptr;
		uint16_t                             _width = 1;
		uint16_t                             _height = 1;
		uint16_t                             _depth = 1;
		uint16_t							 _array_layer_count = 1;
		uint8_t                              _mip_level_count = 1;
		uint8_t                              _flags = 0;    // TextureFlags bitmasks
											 
		VkFormat							 _format = VK_FORMAT_UNDEFINED;
		TextureType::Enum					 _type = TextureType::Texture2D;
		TextureHandle						 _alias = Invalid_Texture;
											 
		const char* 						 _name = nullptr;
		TextureCreation& reset();
		TextureCreation& set_size(uint16_t width, uint16_t height, uint16_t depth);
		TextureCreation& set_flags(uint8_t flags);
		TextureCreation& set_mips(uint32_t mip_level_count);
		TextureCreation& set_layers(uint32_t layer_count);
		TextureCreation& set_format_type(VkFormat format, TextureType::Enum type);
		TextureCreation& set_name(const char* name);
		TextureCreation& set_data(void* data);
		TextureCreation& set_alias(TextureHandle alias);

	}; // struct TextureCreation


	struct TextureSubResource {

		uint16_t                             mip_base_level = 0;
		uint16_t                             mip_level_count = 1;
		uint16_t                             array_base_layer = 0;
		uint16_t                             array_layer_count = 1;

	}; // struct TextureSubResource

	struct TextureViewCreation {
		TextureHandle				_parent_texture = Invalid_Texture;
		VkImageViewType				_view_type = VK_IMAGE_VIEW_TYPE_1D;
		TextureSubResource			_sub_resource;
		
		const char* _name = nullptr;

		TextureViewCreation& reset();
		TextureViewCreation& set_parent_texture(TextureHandle parent_texture);
		TextureViewCreation& set_mips(uint32_t base_mip, uint32_t mip_level_count);
		TextureViewCreation& set_array(uint32_t base_layer, uint32_t layer_count);
		TextureViewCreation& set_name(const char* name);
		TextureViewCreation& set_view_type(VkImageViewType view_type);
		
	};
	//
	//
	struct SamplerCreation {

		VkFilter                        min_filter = VK_FILTER_NEAREST;
		VkFilter                        mag_filter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		const char* name = nullptr;

		SamplerCreation& set_min_mag_mip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip);
		SamplerCreation& set_address_mode_u(VkSamplerAddressMode u);
		SamplerCreation& set_address_mode_uv(VkSamplerAddressMode u, VkSamplerAddressMode v);
		SamplerCreation& set_address_mode_uvw(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w);
		SamplerCreation& set_name(const char* name);

	}; // struct SamplerCreation
	
	//
	//
	struct ShaderStage {

		const char* code = nullptr;
		uint32_t                             code_size = 0;
		VkShaderStageFlagBits           type = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	}; // struct ShaderStage

	//
	//
	struct ShaderStateCreation {

		ShaderStage                     stages[k_max_shader_stages];

		const char* name = nullptr;

		uint32_t                             stages_count = 0;
		uint32_t                             spv_input = 0;

		// Building helpers
		ShaderStateCreation& reset();
		ShaderStateCreation& set_name(const char* name);
		ShaderStateCreation& add_stage(const char* code, uint32_t code_size, VkShaderStageFlagBits type);
		ShaderStateCreation& set_spv_input(bool value);

	}; // struct ShaderStateCreation

	//
	//
	struct DescriptorSetLayoutCreation {

		//
		// A single descriptor binding. It can be relative to one or more resources of the same type.
		//
		struct Binding {

			VkDescriptorType            type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			uint16_t                         start = 0;
			uint16_t                         count = 0;
			std::string                     name = nullptr;  // Comes from external memory.
		}; // struct Binding

		Binding									bindings[k_max_descriptors_per_set];
		uint32_t                             num_bindings = 0;
		uint32_t                             set_index = 0;

		std::string                         name = nullptr;

		// Building helpers
		DescriptorSetLayoutCreation& reset();
		DescriptorSetLayoutCreation& add_binding(const Binding& binding);
		DescriptorSetLayoutCreation& add_binding_at_index(const Binding& binding, int index);
		DescriptorSetLayoutCreation& set_name(std::string name);
		DescriptorSetLayoutCreation& set_set_index(uint32_t index);

	}; // struct DescriptorSetLayoutCreation

	//
	//
	struct DescriptorSetCreation {

		ResourceHandle                  resources[k_max_descriptors_per_set];
		SamplerHandle                   samplers[k_max_descriptors_per_set];
		uint16_t                             bindings[k_max_descriptors_per_set];

		DescriptorSetLayoutHandle       layout;
		uint32_t                             num_resources = 0;

		std::string                         name = nullptr;

		// Building helpers
		DescriptorSetCreation& reset();
		DescriptorSetCreation& set_layout(DescriptorSetLayoutHandle layout);
		DescriptorSetCreation& texture(TextureHandle texture, uint16_t binding);
		DescriptorSetCreation& buffer(BufferHandle buffer, uint16_t binding);
		DescriptorSetCreation& texture_sampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding);   // TODO: separate samplers from textures
		DescriptorSetCreation& set_name(std::string name);

	}; // struct DescriptorSetCreation

	struct RenderPassCreation {

	};

	struct FramebufferCreation {

		RenderPassHandle                render_pass;

		uint16_t                             num_render_targets = 0;

		TextureHandle                   output_textures[k_max_image_outputs];
		TextureHandle                   depth_stencil_texture = { invalidIndex };
		TextureHandle                   shading_rate_attachment = { invalidIndex };

		uint16_t                             width = 0;
		uint16_t                             height = 0;

		float                             scale_x = 1.f;
		float                             scale_y = 1.f;

		uint16_t                             layers = 1;
		uint8_t                              resize = 1;

		const char*                         name = nullptr;

		FramebufferCreation& reset();
		FramebufferCreation& add_render_texture(TextureHandle texture);
		FramebufferCreation& set_depth_stencil_texture(TextureHandle texture);
		FramebufferCreation& add_shading_rate_attachment(TextureHandle texture);
		FramebufferCreation& set_scaling(float scale_x, float scale_y, uint8_t resize);
		FramebufferCreation& set_width_height(uint32_t width, uint32_t height);
		FramebufferCreation& set_layers(uint32_t layers);
		FramebufferCreation& set_name(const char* name);

	}; // struct RenderPassCreation
	//
	struct VertexAttribute {
		uint16_t		_location = 0;
		uint16_t		_binding = 0;
		uint32_t		_offset = 0;
		VertexComponentFormat::Enum _format = VertexComponentFormat::Count;

	};
	//
	struct VertexStream {
		uint16_t		binding = 0;
		uint16_t		stride = 0;
		VertexInputRate::Enum     _input_rate = VertexInputRate::Count;
		
	};

	struct VertexInputCreation {
		uint32_t		_num_vertex_streams = 0;
		uint32_t		_num_vertex_attributes = 0;
		VertexStream	_vertex_streams[k_max_vertex_streams];
		VertexAttribute	_vertex_attributes[k_max_vertex_attributes];

		VertexInputCreation& Reset();
		VertexInputCreation& AddVertexStream(const VertexStream& stream);
		VertexInputCreation& AddVertexAttribute(const VertexAttribute& attribute);

	};

	struct PipelineCreation {
		RasterizationCreation		_rasterization;
		DepthStencilCreation		_depth_stencil;
		BlendStateCreation			_blend_state;
		
	};

	//
	struct ResourceUpdate {

		ResourceUpdateType::Enum        type;
		ResourceHandle                  handle;
		uint32_t                             current_frame;
		uint32_t                             deleting;
	}; // struct ResourceUpdate
	static const uint32_t                    k_max_swapchain_images = 3;
	static const uint32_t                    k_max_frames = 2;
	struct VulkanBuffer : Resource {
	
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

		//const char* _name = nullptr;

	};

	struct VulkanSampler : Resource {

		VkSampler                       _vkSampler;

		VkFilter                        _minFilter = VK_FILTER_NEAREST;
		VkFilter                        _magFilter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode             _mipFilter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerAddressMode            _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode            _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;



		const char* name = nullptr;

	};

	struct VulkanTexture : Resource {

		VkImage                         vk_image;
		VkImageView                     vk_image_view;
		VkFormat                        vk_format;
		VkImageUsageFlags				vk_usage;
		VkImageLayout                   vk_image_layout;
		VmaAllocation                   vma_allocation;
		ResourceState					state = RESOURCE_STATE_UNDEFINED;
		

		uint16_t                        width = 1;
		uint16_t                        height = 1;
		uint16_t                        depth = 1;
		uint16_t						array_layer_count = 1;
		uint8_t							mip_level_count = 1;
		uint8_t                         flags = 0;
		uint16_t						mip_base_level = 0;
		uint16_t						array_base_layer = 0;
		bool							sparse = false;

		TextureHandle                   handle;
		TextureHandle					parent_txture;
		TextureHandle					alias_texture;
		TextureType::Enum               type = TextureType::Texture2D;

		VulkanSampler* sampler = nullptr;

		const char* name = nullptr;
	}; // struct TextureVulkan

	struct VulkanShaderState : Resource {

		VkPipelineShaderStageCreateInfo shader_stage_info[k_max_shader_stages];

		const char* name = nullptr;

		uint32_t                             active_shaders = 0;
		bool                            graphics_pipeline = false;


	}; // struct ShaderStateVulkan

	struct VulkanDescriptorBinding : Resource {

		VkDescriptorType					 type;
		uint16_t                             start = 0;
		uint16_t                             count = 0;
		uint16_t                             set = 0;

		const char* name = nullptr;
	}; // struct ResourceBindingVulkan

	struct VulkanDesciptorSetLayout : Resource {

		VkDescriptorSetLayout					vk_descriptor_set_layout;

		VkDescriptorSetLayoutBinding* vk_binding = nullptr;
		VulkanDescriptorBinding* bindings = nullptr;
		uint16_t								num_bindings = 0;
		uint16_t								set_index = 0;

		DescriptorSetLayoutHandle				handle;

	}; // struct DesciptorSetLayoutVulkan

	struct VulkanDesciptorSet : Resource {

		VkDescriptorSet						vk_descriptor_set;

		ResourceHandle* resources = nullptr;
		SamplerHandle* samplers = nullptr;
		uint16_t* bindings = nullptr;

		const VulkanDesciptorSetLayout* layout = nullptr;
		uint32_t                             num_resources = 0;
	}; // struct DesciptorSetVulkan

	struct VulkanPipeline : Resource {

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

	struct VulkanRenderPass : Resource {

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

	struct VulkanFramebuffer : Resource {

		// NOTE(marco): this will be a null handle if dynamic rendering is available
		VkFramebuffer                        vk_framebuffer;

		// NOTE(marco): cache render pass handle
		RenderPassHandle                     render_pass;

		uint16_t                             width = 0;
		uint16_t                             height = 0;

		float							     scale_x = 1.f;
		float								 scale_y = 1.f;

		TextureHandle						 color_attachments[k_max_image_outputs];
		TextureHandle						 depth_stencil_attachment;
		uint32_t                             num_color_attachments;

		uint8_t                              resize = 0;

		const char* name = nullptr;
	}; // struct Framebuffer



	///////////////////////////////////////////////////////////////////Resource///////////////////////////////////////////////////////////

		//
	namespace TextureFormatted {

		inline bool                     is_depth_stencil(VkFormat value) {
			return value >= VK_FORMAT_D16_UNORM_S8_UINT && value < VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		}
		inline bool                     is_depth_only(VkFormat value) {
			return value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_S8_UINT;
		}
		inline bool                     is_stencil_only(VkFormat value) {
			return value == VK_FORMAT_S8_UINT;
		}

		inline bool                     has_depth(VkFormat value) {
			return is_depth_only(value) || is_depth_stencil(value);
		}
		inline bool                     has_stencil(VkFormat value) {
			return value >= VK_FORMAT_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
		}
		inline bool                     has_depth_or_stencil(VkFormat value) {
			return value >= VK_FORMAT_D16_UNORM && value <= VK_FORMAT_D32_SFLOAT_S8_UINT;
		}

	} // namespace TextureFormat
	//

	struct BufferResource : public Resource {

		BufferHandle                    handle;
		uint32_t                             pool_index;
		

		std::string        k_type = "raptor_buffer_type";
		static uint64_t                      k_type_hash;

	}; // struct Buffer

	//
	//
	struct TextureResource : public Resource {

		TextureHandle							handle;
		uint32_t								pool_index;
		

		std::string								k_type = "raptor_texture_type";
		static uint64_t							k_type_hash;

	}; // struct Texture

	//
	//
	struct SamplerResource : public Resource {

		SamplerHandle                   handle;
		uint32_t                              pool_index;


		std::string       k_type = "raptor_sampler_type";
		static uint64_t                      k_type_hash;

	}; // struct Sampler

	// Material/Shaders /////////////////////////////////////////////////////////////

	//
	//
	struct ProgramPass {

		PipelineHandle                  pipeline;
		DescriptorSetLayoutHandle       descriptor_set_layout;
	}; // struct ProgramPass

	//
	//


	//
	//
	struct Program : public Resource {

		uint32_t                             get_num_passes() const;

		std::vector<ProgramPass>             passes;

		uint32_t                             pool_index;

		std::string							 k_type = "raptor_program_type";
		static uint64_t                      k_type_hash;

	}; // struct Program

	//
	//
	struct MaterialCreation {

		MaterialCreation& reset();
		MaterialCreation& set_program(Program* program);
		MaterialCreation& set_name(std::string name);
		MaterialCreation& set_render_index(uint32_t render_index);

		Program* program = nullptr;
		std::string                         name = nullptr;
		uint32_t                             render_index = ~0u;

	}; // struct MaterialCreation
	// Resource creation structs //////////////////////////////////////////////

//
//

	//
	//
	//
	struct Material : public Resource {

		Program* program;

		uint32_t                             render_index;

		uint32_t                             pool_index;

		std::string							 k_type = "raptor_material_type";
		static uint64_t                      k_type_hash;

	}; // struct Material

	////////////////////update/////////////////////////////////


	VkImageUsageFlags vulkan_get_image_usage(const TextureCreation& creation);

	VkImageType to_vk_image_type(TextureType::Enum type);

	VkImageViewType to_vk_image_view_type(TextureType::Enum type);

	VkAccessFlags util_to_vk_access_flags2(ResourceState state);

	VkAccessFlags util_to_vk_access_flags(ResourceState state);

	VkPipelineStageFlags2KHR util_determine_pipeline_stage_flags2(VkAccessFlags2KHR access_flags, QueueType::Enum queue_type);

	VkPipelineStageFlags util_determine_pipeline_stage_flags(VkAccessFlags access_flags, QueueType::Enum queue_type);

	VkImageLayout util_to_vk_image_layout2(ResourceState usage);

	VkImageLayout util_to_vk_image_layout(ResourceState usage);

	
}

