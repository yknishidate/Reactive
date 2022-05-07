#include "Vulkan.hpp"
#include "Mesh.hpp"
#include "Loader.hpp"
#include <spdlog/spdlog.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Material material,
           vk::GeometryFlagBitsKHR geomertyFlag)
{
    this->material = material;
    vk::BufferUsageFlags usage{
    vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress
    };

    vertexBuffer.InitOnHost(sizeof(Vertex) * vertices.size(), usage);
    indexBuffer.InitOnHost(sizeof(uint32_t) * indices.size(), usage);
    vertexBuffer.Copy(vertices.data());
    indexBuffer.Copy(indices.data());
    bottomAccel.Init(vertexBuffer, indexBuffer, vertices.size(), indices.size() / 3, geomertyFlag);
}

Mesh::Mesh(const std::string& filepath, vk::GeometryFlagBitsKHR geomertyFlag)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Loader::LoadFromFile(filepath, vertices, indices);

    vk::BufferUsageFlags usage{
        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
        vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress
    };

    vertexBuffer.InitOnHost(sizeof(Vertex) * vertices.size(), usage);
    indexBuffer.InitOnHost(sizeof(uint32_t) * indices.size(), usage);
    vertexBuffer.Copy(vertices.data());
    indexBuffer.Copy(indices.data());
    bottomAccel.Init(vertexBuffer, indexBuffer, vertices.size(), indices.size() / 3, geomertyFlag);
}
