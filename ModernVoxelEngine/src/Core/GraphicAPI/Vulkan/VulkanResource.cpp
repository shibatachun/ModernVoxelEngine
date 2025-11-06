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


vulkan::BufferCreation& vulkan::BufferCreation::set_name(const char* name_) {
    name = name_;
    return *this;
}

vulkan::TextureViewCreation& vulkan::TextureViewCreation::set_view_type(VkImageViewType view_type)
{
    _view_type = view_type;
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
