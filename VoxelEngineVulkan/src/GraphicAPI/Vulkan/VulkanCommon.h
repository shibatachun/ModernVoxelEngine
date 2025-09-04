#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define NOMINMAX
#pragma comment(lib, "vulkan-1.lib")
#include <vulkan/vulkan.h>

#define VULKAN_NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	VulkanHandleType Handle() const { return name; } \
private: \
	VulkanHandleType name{};\


#include <chrono>



#include "../../ResourcesManager/AssetManager.h"
#include "../../utils/minilog.h"
#include "../../utils/Pool.h"
#include "../../utils/Utils.h"


constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

namespace vulkan {

	
	enum class VertexComponent { Position, Normal, UV, Color, Tangent, Joint0, Weight0 };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

	struct graphicsPipelineCreateInfoPack {
		VkGraphicsPipelineCreateInfo createInfo =
		{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		//Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		std::vector<VkVertexInputBindingDescription> vertexInputBindings;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
		//Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		//Tessellation
		VkPipelineTessellationStateCreateInfo tessellationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
		//Viewport
		VkPipelineViewportStateCreateInfo viewportStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;
		uint32_t dynamicViewportCount = 1;//动态视口/剪裁不会用到上述的vector，因此动态视口和剪裁的个数向这俩变量手动指定
		uint32_t dynamicScissorCount = 1;
		//Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		//Multisample
		VkPipelineMultisampleStateCreateInfo multisampleStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		//Depth & Stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		//Color Blend
		VkPipelineColorBlendStateCreateInfo colorBlendStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
		//Dynamic
		VkPipelineDynamicStateCreateInfo dynamicStateCi =
		{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		std::vector<VkDynamicState> dynamicStates;
		//--------------------
		graphicsPipelineCreateInfoPack() {
			SetCreateInfos();
			//若非派生管线，createInfo.basePipelineIndex不得为0，设置为-1
			createInfo.basePipelineIndex = -1;
		}
		//移动构造器，所有指针都要重新赋值
		graphicsPipelineCreateInfoPack(const graphicsPipelineCreateInfoPack& other) noexcept {
			createInfo = other.createInfo;
			SetCreateInfos();

			vertexInputStateCi = other.vertexInputStateCi;
			inputAssemblyStateCi = other.inputAssemblyStateCi;
			tessellationStateCi = other.tessellationStateCi;
			viewportStateCi = other.viewportStateCi;
			rasterizationStateCi = other.rasterizationStateCi;
			multisampleStateCi = other.multisampleStateCi;
			depthStencilStateCi = other.depthStencilStateCi;
			colorBlendStateCi = other.colorBlendStateCi;
			dynamicStateCi = other.dynamicStateCi;

			shaderStages = other.shaderStages;
			vertexInputBindings = other.vertexInputBindings;
			vertexInputAttributes = other.vertexInputAttributes;
			viewports = other.viewports;
			scissors = other.scissors;
			colorBlendAttachmentStates = other.colorBlendAttachmentStates;
			dynamicStates = other.dynamicStates;
			UpdateAllArrayAddresses();
		}
		//Getter，这里我没用const修饰符
		operator const VkGraphicsPipelineCreateInfo& () { return createInfo; }
		//Non-const Function
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，并相应改变各个count
		void UpdateAllArrays() {
			createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			vertexInputStateCi.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
			vertexInputStateCi.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
			viewportStateCi.viewportCount = uint32_t(viewports.size()) ? uint32_t(viewports.size()) : dynamicViewportCount;
			viewportStateCi.scissorCount = uint32_t(scissors.size()) ? uint32_t(scissors.size()) : dynamicScissorCount;
			colorBlendStateCi.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
			dynamicStateCi.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			UpdateAllArrayAddresses();
		}
	private:
		//该函数用于将创建信息的地址赋值给basePipelineIndex中相应成员
		void SetCreateInfos() {
			createInfo.pVertexInputState = &vertexInputStateCi;
			createInfo.pInputAssemblyState = &inputAssemblyStateCi;
			createInfo.pTessellationState = &tessellationStateCi;
			createInfo.pViewportState = &viewportStateCi;
			createInfo.pRasterizationState = &rasterizationStateCi;
			createInfo.pMultisampleState = &multisampleStateCi;
			createInfo.pDepthStencilState = &depthStencilStateCi;
			createInfo.pColorBlendState = &colorBlendStateCi;
			createInfo.pDynamicState = &dynamicStateCi;
		}
		//该函数用于将各个vector中数据的地址赋值给各个创建信息中相应成员，但不改变各个count
		void UpdateAllArrayAddresses() {
			createInfo.pStages = shaderStages.data();
			vertexInputStateCi.pVertexBindingDescriptions = vertexInputBindings.data();
			vertexInputStateCi.pVertexAttributeDescriptions = vertexInputAttributes.data();
			viewportStateCi.pViewports = viewports.data();
			viewportStateCi.pScissors = scissors.data();
			colorBlendStateCi.pAttachments = colorBlendAttachmentStates.data();
			dynamicStateCi.pDynamicStates = dynamicStates.data();
		}
	};

	struct LayoutConfig {
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		uint32_t bindingCounts;
		// 如果还有 push constant ranges，也可以放在这里：
		// std::vector<VkPushConstantRange> pushConstants;

		// 比较运算符：先比大小，再逐一比较字段
		bool operator==(LayoutConfig const& o) const {
			if (bindings.size() != o.bindings.size()) return false;
			for (size_t i = 0; i < bindings.size(); i++) {
				const auto& a = bindings[i];
				const auto& b = o.bindings[i];
				if (a.binding != b.binding ||
					a.descriptorType != b.descriptorType ||
					a.descriptorCount != b.descriptorCount ||
					a.stageFlags != b.stageFlags ||
					a.pImmutableSamplers != b.pImmutableSamplers   // 通常 nullptr
					) return false;
			}
			// 同理比较 pushConstants…
			return true;
		}
		void UpdateAllArray() {
			bindingCounts = static_cast<uint32_t>((bindings.size()));
		}
	};

	inline void hashCombine(std::size_t& seed, std::size_t v) {
		seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	}

	struct LayoutConfigHash {
		std::size_t operator()(LayoutConfig const& cfg) const noexcept {
			std::size_t h = 0;
			for (auto const& b : cfg.bindings) {
				// 把每个字段都混合进 seed
				hashCombine(h, std::hash<uint32_t>()(b.binding));
				hashCombine(h, std::hash<uint32_t>()(b.descriptorType));
				hashCombine(h, std::hash<uint32_t>()(b.descriptorCount));
				hashCombine(h, std::hash<uint32_t>()(b.stageFlags));
				// pImmutableSamplers 通常都是 nullptr 才会相等，否则需要根据地址或内容再 hash
				hashCombine(h, std::hash<uintptr_t>()(
					reinterpret_cast<uintptr_t>(b.pImmutableSamplers)));
			}
			// 如果有 pushConstants，再把它们也混合进 h
			// for (auto const &p : cfg.pushConstants) { … }
			return h;
		}
	};

	struct PipelineEntry
	{
		std::string pipeline;
		std::string layout;
	};

    enum class QueueFamily
    {
        GRAPHIC,
        COMPUTE,
        TRANSFER
    };

    inline const char* ToString(const VkResult result)
    {
        switch (result)
        {
#define STR(r) case VK_ ##r: return #r
            STR(SUCCESS);
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_FRAGMENTED_POOL);
            STR(ERROR_UNKNOWN);
            STR(ERROR_OUT_OF_POOL_MEMORY);
            STR(ERROR_INVALID_EXTERNAL_HANDLE);
            STR(ERROR_FRAGMENTATION);
            STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
            STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
            STR(ERROR_NOT_PERMITTED_EXT);
            STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
            STR(THREAD_IDLE_KHR);
            STR(THREAD_DONE_KHR);
            STR(OPERATION_DEFERRED_KHR);
            STR(OPERATION_NOT_DEFERRED_KHR);
            STR(PIPELINE_COMPILE_REQUIRED_EXT);
#undef STR
        default:
            return "UNKNOWN_ERROR";
        }
    }

    inline void Check(const VkResult result, const char* const operation)
    {
        if (result != VK_SUCCESS)
        {
            std::cerr << "Error: failed to " << operation
                << " (" << ToString(result) << ")\n";
            throw std::runtime_error(
                std::string("failed to ") + operation +
                " (" + ToString(result) + ")"
            );
        }
        else
        {
            std::cout << "Success: do " << operation << " success!" << std::endl;
        }
    }

    inline VkVertexInputBindingDescription getBindingDescription(){
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex1);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

	VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);
	
	VkVertexInputAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
	
	std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);

	void getPipelineVertexInputState(const std::vector<VertexComponent> components,  graphicsPipelineCreateInfoPack& createinfo);

	inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
		VkDescriptorType type,
		VkShaderStageFlags stageFlags,
		uint32_t binding,
		uint32_t descriptorCount = 1)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding{};
		setLayoutBinding.descriptorType = type;
		setLayoutBinding.stageFlags = stageFlags;
		setLayoutBinding.binding = binding;
		setLayoutBinding.descriptorCount = descriptorCount;
		return setLayoutBinding;
	}

	struct VulkanTexture {
		VkSampler sampler{ VK_NULL_HANDLE };
		VkImage image{ VK_NULL_HANDLE };
		VkImageLayout imageLayout;
		VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
		VkImageView view{ VK_NULL_HANDLE };
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		uint32_t mipLevels{ 0 };
	};

	struct VulkanRenderObject {
		std::string name;
		VkPipeline pipeline;
		VkPipelineLayout Pipelinelayout;
		std::vector<VulkanTexture> textures;
		VkBuffer		vertexBuffer;
		VkDeviceMemory	vertexmemory;

		VkBuffer		indiceBuffer;
		VkDeviceMemory	indicememory;

		std::vector<uint32_t>		indiceCounts;
	};


	namespace VulkanResource {
	

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

		void createImage(uint32_t width, uint32_t height, VkFormat format,
			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage& image, VkDeviceMemory& imageMemory, VkDevice& device, VkPhysicalDevice phy_device);

		void createTextureImage();

	

	}
	



}