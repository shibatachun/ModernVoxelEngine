#include "VulkanCommon.h"

VkVertexInputBindingDescription vulkan::inputBindingDescription(uint32_t binding)
{
	return VkVertexInputBindingDescription({ binding, sizeof(Vertex1),VK_VERTEX_INPUT_RATE_VERTEX });
}


VkVertexInputAttributeDescription vulkan::inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component)
{
	switch (component) {
	case VertexComponent::Position:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex1, pos) });
	case VertexComponent::Normal:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex1, normal) });
	case VertexComponent::UV:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex1, uv) });
	case VertexComponent::Color:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex1, color) });
	case VertexComponent::Tangent:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex1, tangent) });
	case VertexComponent::Joint0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex1, joint0) });
	case VertexComponent::Weight0:
		return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex1, weight0) });
	default:
		return VkVertexInputAttributeDescription({});
	}
}

std::vector<VkVertexInputAttributeDescription> vulkan::inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components)
{
	std::vector<VkVertexInputAttributeDescription> result;
	uint32_t location = 0;
	for (VertexComponent component : components) {
		result.push_back(inputAttributeDescription(binding, location, component));
		location++;
	}
	return result;
}

void vulkan::getPipelineVertexInputState(const std::vector<VertexComponent> components, graphicsPipelineCreateInfoPack& createinfo)
{
	
	createinfo.vertexInputAttributes = inputAttributeDescriptions(0, components);
	createinfo.vertexInputBindings = { inputBindingDescription(0) };
	createinfo.UpdateAllArrays();


	//createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//createinfo.vertexBindingDescriptionCount = 1;
	//createinfo.pVertexBindingDescriptions = &vertexDescription;
	//createinfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribute.size());
	//createinfo.pVertexAttributeDescriptions = vertexAttribute.data();

}
