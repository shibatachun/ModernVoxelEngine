#include "VulkanResource.h"

vulkan::BlendState& vulkan::BlendState::set_color(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation)
{
    BlendState state{};
    return state;
}

vulkan::BlendState& vulkan::BlendState::set_alpha(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation)
{
    BlendState state{};
    return state;

}

vulkan::BlendState& vulkan::BlendState::set_color_write_mask(ColorWriteEnabled::Mask value)
{
    BlendState state{};
    return state;
}

vulkan::DepthStencilCreation& vulkan::DepthStencilCreation::set_depth(bool write, VkCompareOp comparison_test)
{
    DepthStencilCreation state{};
    return state;
}

vulkan::BlendState& vulkan::BlendStateCreation::add_blend_state()
{
    BlendState state{};
    return state;
}
