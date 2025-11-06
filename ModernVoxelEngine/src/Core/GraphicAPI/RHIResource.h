#pragma once
#include "../../ResourcesManager/AssetManager.h"
#include "../../utils/minilog.h"
#include "../../utils/Pool.h"
#include "../../utils/Utils.h"
#include "../../utils/Camera.h"


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


	struct ResourceInfo {

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

	struct FramebufferHandle {
		ResourceHandle                  index;
	}; // struct FramebufferHandle

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


	namespace TextureFlags {
		enum Enum {
			Default, RenderTarget, Compute, Sparse, ShadingRate, Count
		};

		enum Mask {
			Default_mask = 1 << 0, RenderTarget_mask = 1 << 1, Compute_mask = 1 << 2, Sparse_mask = 1 << 3, ShadingRate_mask = 1 << 4
		};

		static const char* s_value_names[] = {
			"Default", "RenderTarget", "Compute", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}

	} // namespace TextureFlags
	namespace TextureType {
		enum Enum {
			Texture1D, Texture2D, Texture3D, TextureCube, Texture_1D_Array, Texture_2D_Array, Texture_Cube_Array, Count
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
	namespace ResourceUpdateType {

		enum Enum {
			Buffer, Texture, Pipeline, Sampler, DescriptorSetLayout, DescriptorSet, RenderPass, Framebuffer, ShaderState, TextureView, PagePool, Count
		};

		static const char* s_value_names[] = {
			"Buffer", "Texture", "Pipeline", "Sampler", "DescriptorSetLayout", "DescriptorSet", "RenderPass", "Framebuffer", "ShaderState", "TextureView", "PagePool"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace ResourceUpdateType
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

	namespace VertexInputRate {
		enum Enum {
			PerVertex, PerInstance, Count
		};

		enum Mask {
			PerVertex_mask = 1 << 0, PerInstance_mask = 1 << 1, Count_mask = 1 << 2
		};

		static const char* s_value_names[] = {
			"PerVertex", "PerInstance", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	} // namespace VertexInputRate

	namespace VertexComponentFormat {
		enum Enum {
			Float, Float2, Float3, Float4, Mat4, Byte, Byte4N, UByte, UByte4N, Short2, Short2N, Short4, Short4N, Uint, Uint2, Uint4, Count
		};

		static const char* s_value_names[] = {
			"Float", "Float2", "Float3", "Float4", "Mat4", "Byte", "Byte4N", "UByte", "UByte4N", "Short2", "Short2N", "Short4", "Short4N", "Uint", "Uint2", "Uint4", "Count"
		};

		static const char* ToString(Enum e) {
			return ((uint32_t)e < Enum::Count ? s_value_names[(int)e] : "unsupported");
		}
	}

	namespace RenderPassType {

		enum Enum {
			Geometry, Swapchain, Compute
		};
	} // namespace RenderPassType
	template <typename T>
	struct ResourcePool {
		void init(uint32_t pool_size) {
			poolSize = pool_size;
			resources.resize(pool_size);
			freeIndices.resize(pool_size);
			for (uint32_t i = 0; i < pool_size; ++i) {
				freeIndices[i] = i;
			}
			freeIndicesHead = 0;
			usedIndices = 0;
		}
	

		uint32_t obtain_resouce() {
			if (freeIndicesHead < poolSize) {
				const uint32_t freeIndex = freeIndices[freeIndicesHead++];
				--usedIndices;

				return freeIndex;
			}
			return invalidIndex;
		}
		T* obtain() {
			uint32_t resourceIndex = obtain_resouce();
			if (resourceIndex != invalidIndex) {
				T* resource = get(resourceIndex);
				resource->poolIndex = resourceIndex;
				return resource;
			}
			return nullptr;
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

		void shutdown() {
			std::vector<T>().swap(resources);
			std::vector<uint32_t>().swap(freeIndices);
		}

	private:
		uint32_t poolSize;
		std::vector<T> resources;
		std::vector<uint32_t> freeIndices;
		uint32_t usedIndices;
		uint32_t freeIndicesHead;
	private:
	
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



////RHI Resouce

