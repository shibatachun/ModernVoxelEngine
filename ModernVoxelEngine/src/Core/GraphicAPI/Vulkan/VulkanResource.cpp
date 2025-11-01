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
