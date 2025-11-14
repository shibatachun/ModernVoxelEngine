#include "VulkanResource.h"

vulkan::VertexInputCreation& vulkan::VertexInputCreation::Reset()
{
    _num_vertex_streams = _num_vertex_attributes = 0;
    return *this;
}

vulkan::VertexInputCreation& vulkan::VertexInputCreation::AddVertexStream(const VertexStream& stream)
{
    _vertex_streams[_num_vertex_streams++] = stream;
    return *this;
}

vulkan::VertexInputCreation& vulkan::VertexInputCreation::AddVertexAttribute(const VertexAttribute& attribute)
{
    _vertex_attributes[_num_vertex_attributes++] = attribute;
    return *this;
}

vulkan::BufferCreation& vulkan::BufferCreation::set_name(const char* name_) {
    _name = name_;
    return *this;
}
vulkan::BufferCreation& vulkan::BufferCreation::set(VkBufferUsageFlags flags_, ResourceUsageType::Enum usage_, uint32_t size_)
{   
    type_flags = flags_;
    usage = usage_;
    size = size_;
    return *this;

}

vulkan::BufferCreation& vulkan::BufferCreation::set_data(void* data_)
{
    initial_data = data_;
    return *this;
}


vulkan::BufferCreation& vulkan::BufferCreation::set_persistent(bool value)
{
    persistent = value;
    return *this;
}

vulkan::BufferCreation& vulkan::BufferCreation::set_device_only(bool value)
{
    device_only = value;
    return *this;
}
//////////////////////////////////////TextureViewCreationMethods/////////////////////////////////////////////////////////
vulkan::TextureViewCreation& vulkan::TextureViewCreation::reset() {
    _parent_texture = Invalid_Texture;
    _sub_resource = { 0,1,0,1 };
    _name = nullptr;
    _view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    return *this;
}
vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_parent_texture(TextureHandle parent_texture)
{
    _parent_texture = parent_texture;
    return *this;
}

vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_mips(uint32_t base_mip, uint32_t mip_level_count)
{
    _sub_resource.mip_base_level = base_mip;
    _sub_resource.mip_level_count = mip_level_count;
    return *this;
}

vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_array(uint32_t base_layer, uint32_t layer_count)
{
    _sub_resource.array_base_layer = base_layer;
    _sub_resource.array_layer_count = layer_count;
    return *this;
}

vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_name(const char* name_) {
    _name = name_;
    return *this;
}

vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_view_type(VkImageViewType view_type)
{
    _view_type = view_type;
    return *this;
}


/////////////////////////////////////TxtureCreationMethod//////////////////////////////////////////////////////////////
vulkan::TextureCreation& vulkan::TextureCreation::reset()
{
    _mip_level_count = 1;
    _array_layer_count = 1;
    _initial_data = nullptr;
    _alias = Invalid_Texture;

    _width = _height = _depth = 1;
    _format = VK_FORMAT_UNDEFINED;
    _flags = 0;
    
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_data(void* data)
{
    _initial_data = data;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_size(uint16_t width, uint16_t height, uint16_t depth)
{
    _width = width;
    _height = height;
    _depth = depth;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_flags(uint8_t flags)
{
    _flags = flags;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_mips(uint32_t mip_level_count)
{
    _mip_level_count = mip_level_count;

    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_layers(uint32_t layer_count)
{
    _array_layer_count = layer_count;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_format_type(VkFormat format, TextureType::Enum type)
{
    _format = format;
    _type = type;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_alias(TextureHandle alias)
{
    _alias = alias;
    return *this;
}

vulkan::TextureCreation& vulkan::TextureCreation::set_name(const char* name)
{
    _name = name;
    return *this;
}




VkImageUsageFlags vulkan::vulkan_get_image_usage(const vulkan::TextureCreation& creation) {
    const bool is_render_target = (creation._flags & TextureFlags::RenderTarget_mask) == TextureFlags::RenderTarget_mask;
    const bool is_compute_used = (creation._flags & TextureFlags::Compute_mask) == TextureFlags::Compute_mask;
    const bool is_shading_rate_texture = (creation._flags & TextureFlags::ShadingRate_mask) == TextureFlags::ShadingRate_mask;

    // Default to always readable from shader.
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;

    usage |= is_compute_used ? VK_IMAGE_USAGE_STORAGE_BIT : 0;

    usage |= is_shading_rate_texture ? VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;

    if (vulkan::TextureFormatted::has_depth_or_stencil(creation._format)) {
        // Depth/Stencil textures are normally textures you render into.
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // TODO

    }
    else {
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // TODO
        usage |= is_render_target ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
    }

    return usage;
}

VkImageType vulkan::to_vk_image_type(TextureType::Enum type) {
    static VkImageType s_vk_target[TextureType::Count] = { VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D, VK_IMAGE_TYPE_3D, VK_IMAGE_TYPE_2D, VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D, VK_IMAGE_TYPE_2D };
    return s_vk_target[type];
}

VkImageViewType vulkan::to_vk_image_view_type(TextureType::Enum type)
{
    
        static VkImageViewType s_vk_data[] = { VK_IMAGE_VIEW_TYPE_1D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_VIEW_TYPE_3D, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_VIEW_TYPE_1D_ARRAY, VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY };
        return s_vk_data[type];
    
}

VkAccessFlags vulkan::util_to_vk_access_flags2(ResourceState state)
{
    VkAccessFlags ret = 0;
    if (state & RESOURCE_STATE_COPY_SOURCE) {
        ret |= VK_ACCESS_2_TRANSFER_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_COPY_DEST) {
        ret |= VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
    }
    if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) {
        ret |= VK_ACCESS_2_UNIFORM_READ_BIT_KHR | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_INDEX_BUFFER) {
        ret |= VK_ACCESS_2_INDEX_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_UNORDERED_ACCESS) {
        ret |= VK_ACCESS_2_SHADER_READ_BIT_KHR | VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
    }
    if (state & RESOURCE_STATE_INDIRECT_ARGUMENT) {
        ret |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_RENDER_TARGET) {
        ret |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR;
    }
    if (state & RESOURCE_STATE_DEPTH_WRITE) {
        ret |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_SHADER_RESOURCE) {
        ret |= VK_ACCESS_2_SHADER_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_PRESENT) {
        ret |= VK_ACCESS_2_MEMORY_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_SHADING_RATE_SOURCE) {
        ret |= VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
    }
    if (state & RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) {
        ret |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    }

    return ret;
}

VkPipelineStageFlags2KHR vulkan::util_determine_pipeline_stage_flags2(VkAccessFlags2KHR access_flags, QueueType::Enum queue_type)
{
    VkPipelineStageFlags2KHR flags = 0;

    switch (queue_type) {
    case QueueType::Graphics:
    {
        if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR;

        if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
            flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR;
            flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR;
            flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;

            // TODO(marco): check RT extension is present/enabled
            flags |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
        }

        if ((access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
            flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR;

        if ((access_flags & (VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)) != 0)
            flags |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

        if ((access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;

        if ((access_flags & VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR) != 0)
            flags = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        if ((access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;

        break;
    }
    case QueueType::Compute:
    {
        if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
            (access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
            (access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
            (access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
            return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;

        if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;

        break;
    }
    case QueueType::CopyTransfer: return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
    default: break;
    }

    // Compatible with both compute and graphics queues
    if ((access_flags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
        flags |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR;

    if ((access_flags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
        flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;

    if ((access_flags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
        flags |= VK_PIPELINE_STAGE_2_HOST_BIT_KHR;

    if (flags == 0)
        flags = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;

    return flags;
}

VkPipelineStageFlags vulkan::util_determine_pipeline_stage_flags(VkAccessFlags access_flags, QueueType::Enum queue_type)
{
    VkPipelineStageFlags flags = 0;

    switch (queue_type) {
    case QueueType::Graphics:
    {
        if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

        if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
            flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

            // TODO(marco): check RT extension is present/enabled
            //flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
        }

        if ((access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
            flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        if ((access_flags & (VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)) != 0)
            flags |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

        if ((access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        if ((access_flags & VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR) != 0)
            flags = VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        if ((access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        break;
    }
    case QueueType::Compute:
    {
        if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
            (access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
            (access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
            (access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        break;
    }
    case QueueType::CopyTransfer: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    default: break;
    }

    // Compatible with both compute and graphics queues
    if ((access_flags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
        flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

    if ((access_flags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
        flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

    if ((access_flags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
        flags |= VK_PIPELINE_STAGE_HOST_BIT;

    if (flags == 0)
        flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    return flags;
}

VkImageLayout vulkan::util_to_vk_image_layout2(ResourceState usage)
{
    if (usage & RESOURCE_STATE_COPY_SOURCE)
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    if (usage & RESOURCE_STATE_COPY_DEST)
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    if (usage & RESOURCE_STATE_RENDER_TARGET)
        return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;

    if (usage & RESOURCE_STATE_DEPTH_WRITE)
        return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;

    if (usage & RESOURCE_STATE_DEPTH_READ)
        return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR;

    if (usage & RESOURCE_STATE_UNORDERED_ACCESS)
        return VK_IMAGE_LAYOUT_GENERAL;

    if (usage & RESOURCE_STATE_SHADER_RESOURCE)
        return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR;

    if (usage & RESOURCE_STATE_PRESENT)
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    if (usage == RESOURCE_STATE_COMMON)
        return VK_IMAGE_LAYOUT_GENERAL;

    if (usage == RESOURCE_STATE_SHADING_RATE_SOURCE)
        return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout vulkan::util_to_vk_image_layout(ResourceState usage)
{
    if (usage & RESOURCE_STATE_COPY_SOURCE)
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    if (usage & RESOURCE_STATE_COPY_DEST)
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    if (usage & RESOURCE_STATE_RENDER_TARGET)
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    if (usage & RESOURCE_STATE_DEPTH_WRITE)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    if (usage & RESOURCE_STATE_DEPTH_READ)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    if (usage & RESOURCE_STATE_UNORDERED_ACCESS)
        return VK_IMAGE_LAYOUT_GENERAL;

    if (usage & RESOURCE_STATE_SHADER_RESOURCE)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if (usage & RESOURCE_STATE_PRESENT)
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    if (usage == RESOURCE_STATE_COMMON)
        return VK_IMAGE_LAYOUT_GENERAL;

    if (usage == RESOURCE_STATE_SHADING_RATE_SOURCE)
        return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkAccessFlags vulkan::util_to_vk_access_flags(ResourceState state)
{
    VkAccessFlags ret = 0;
    if (state & RESOURCE_STATE_COPY_SOURCE) {
        ret |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (state & RESOURCE_STATE_COPY_DEST) {
        ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) {
        ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    if (state & RESOURCE_STATE_INDEX_BUFFER) {
        ret |= VK_ACCESS_INDEX_READ_BIT;
    }
    if (state & RESOURCE_STATE_UNORDERED_ACCESS) {
        ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_INDIRECT_ARGUMENT) {
        ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    }
    if (state & RESOURCE_STATE_RENDER_TARGET) {
        ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (state & RESOURCE_STATE_DEPTH_WRITE) {
        ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if (state & RESOURCE_STATE_SHADER_RESOURCE) {
        ret |= VK_ACCESS_SHADER_READ_BIT;
    }
    if (state & RESOURCE_STATE_PRESENT) {
        ret |= VK_ACCESS_MEMORY_READ_BIT;
    }
    if (state & RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) {
        ret |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    }

    return ret;
}
