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
