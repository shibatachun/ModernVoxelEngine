#pragma once
#include "VulkanCommon.h"


#include "vma/vk_mem_alloc.h"
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


	struct ResourceInfo{
		
	};

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

	static BufferHandle                Invalid_Buffer{ invalidIndex };
	static TextureHandle               Invalid_Texture{ invalidIndex };
	static ShaderStateHandle           Invalid_Shader{ invalidIndex };
	static SamplerHandle               Invalid_Sampler{ invalidIndex };
	static DescriptorSetLayoutHandle   Invalid_Layout{ invalidIndex };
	static DescriptorSetHandle         Invalid_Set{ invalidIndex };
	static PipelineHandle              Invalid_Pipeline{ invalidIndex };
	static RenderPassHandle            Invalid_Pass{ invalidIndex };
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

	struct Resource {

		void				add_reference() { ++references; }
		void				remove_reference() { assert(references != 0); --references; }
		uint32_t			poolIndex;

		uint64_t            references = 0;
		std::string         name = nullptr;

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
		void* initial_data = nullptr;

		const char* name = nullptr;

		BufferCreation& reset();
		BufferCreation& set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage, uint32_t size);
		BufferCreation& set_data(void* data);
		BufferCreation& set_name(const char* name);

	}; // struct BufferCreation

	//
	//
	struct TextureCreation {

		void* initial_data = nullptr;
		uint16_t                             width = 1;
		uint16_t                             height = 1;
		uint16_t                             depth = 1;
		uint8_t                              mipmaps = 1;
		uint8_t                              flags = 0;    // TextureFlags bitmasks

		VkFormat                        format = VK_FORMAT_UNDEFINED;
		TextureType::Enum               type = TextureType::Texture2D;

		const char* name = nullptr;

		TextureCreation& set_size(uint16_t width, uint16_t height, uint16_t depth);
		TextureCreation& set_flags(uint8_t mipmaps, uint8_t flags);
		TextureCreation& set_format_type(VkFormat format, TextureType::Enum type);
		TextureCreation& set_name(const char* name);
		TextureCreation& set_data(void* data);

	}; // struct TextureCreation

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

		const char* _name = nullptr;

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
	template <typename T>
	struct ResourcePool {
		void init(uint32_t pool_size) {
			resources.resize(pool_size);
			freeIndices.resize(pool_size);
		
			

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
			return (T*)access_resource(index);
		}
		const T* get(uint32_t index) const {
			return (const T*)access_resource(index);
		}

	
		void free_all_resource() {
			freeIndicesHead = 0;
			usedIndices = 0;
			for (uint32_t i = 0; i < poolSize; i++) {
				freeIndices[i] = i;
			}
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

