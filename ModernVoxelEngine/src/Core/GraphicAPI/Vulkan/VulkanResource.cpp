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

vulkan::BufferCreation& vulkan::BufferCreation::set_name(const char* name_) {
    name = name_;
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
