#include "Context.hpp"
#include "Accel.hpp"
#include "Object.hpp"

namespace
{
    vk::UniqueAccelerationStructureKHR CreateAccel(vk::Buffer buffer, vk::DeviceSize size,
                                                   vk::AccelerationStructureTypeKHR type)
    {
        const auto accelInfo = vk::AccelerationStructureCreateInfoKHR()
            .setBuffer(buffer)
            .setSize(size)
            .setType(type);

        return Context::GetDevice().createAccelerationStructureKHRUnique(accelInfo);
    }

    void BuildAccel(vk::AccelerationStructureKHR accel, vk::DeviceSize size, uint32_t primitiveCount,
                    vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo)
    {
        StorageBuffer scratchBuffer{ size };

        geometryInfo.setScratchData(scratchBuffer.GetAddress());
        geometryInfo.setDstAccelerationStructure(accel);

        Context::OneTimeSubmit(
            [&](vk::CommandBuffer commandBuffer)
            {
                vk::AccelerationStructureBuildRangeInfoKHR buildRangeInfo{ primitiveCount, 0, 0, 0 };
                commandBuffer.buildAccelerationStructuresKHR(geometryInfo, &buildRangeInfo);
            });
    }
}

BottomAccel::BottomAccel(const Mesh& mesh, vk::GeometryFlagBitsKHR geomertyFlag)
{
    const auto triangleData = vk::AccelerationStructureGeometryTrianglesDataKHR()
        .setVertexFormat(vk::Format::eR32G32B32Sfloat)
        .setVertexData(mesh.GetVertexBufferAddress())
        .setVertexStride(sizeof(Vertex))
        .setMaxVertex(mesh.GetVertices().size())
        .setIndexType(vk::IndexType::eUint32)
        .setIndexData(mesh.GetIndexBufferAddress());

    const size_t primitiveCount = mesh.GetIndices().size() / 3;
    const TrianglesGeometry geometry{ triangleData, geomertyFlag, primitiveCount };
    const auto size = geometry.GetAccelSize();
    const auto type = vk::AccelerationStructureTypeKHR::eBottomLevel;

    buffer = geometry.CreateAccelBuffer();
    accel = CreateAccel(buffer.GetBuffer(), size, type);
    BuildAccel(*accel, size, primitiveCount, geometry.GetInfo());
}

TopAccel::TopAccel(const std::vector<Object>& objects, vk::GeometryFlagBitsKHR geomertyFlag)
{
    const uint32_t primitiveCount = objects.size();

    std::vector<vk::AccelerationStructureInstanceKHR> instances;
    for (auto&& object : objects) {
        const auto instance = vk::AccelerationStructureInstanceKHR()
            .setTransform(object.transform.GetVkMatrix())
            .setMask(0xFF)
            .setAccelerationStructureReference(object.GetMesh().GetAccel().GetBufferAddress())
            .setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable);

        instances.push_back(instance);
    }

    instanceBuffer = DeviceBuffer{ vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
                                   vk::BufferUsageFlagBits::eShaderDeviceAddress |
                                   vk::BufferUsageFlagBits::eTransferDst,
                                   instances };

    const auto instancesData = vk::AccelerationStructureGeometryInstancesDataKHR()
        .setArrayOfPointers(false)
        .setData(instanceBuffer.GetAddress());

    geometry = InstancesGeometry{ instancesData, geomertyFlag, primitiveCount };
    const auto size = geometry.GetAccelSize();

    buffer = geometry.CreateAccelBuffer();
    accel = CreateAccel(buffer.GetBuffer(), size, vk::AccelerationStructureTypeKHR::eTopLevel);
    BuildAccel(*accel, size, primitiveCount, geometry.GetInfo());
}

TopAccel::TopAccel(const Object& object, vk::GeometryFlagBitsKHR geomertyFlag)
    : TopAccel(std::vector{ object }, geomertyFlag)
{
}

void TopAccel::Rebuild(const std::vector<Object>& objects)
{
    const uint32_t primitiveCount = objects.size();

    std::vector<vk::AccelerationStructureInstanceKHR> instances;
    for (auto&& object : objects) {
        const auto instance = vk::AccelerationStructureInstanceKHR()
            .setTransform(object.transform.GetVkMatrix())
            .setMask(0xFF)
            .setAccelerationStructureReference(object.GetMesh().GetAccel().GetBufferAddress())
            .setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable);

        instances.push_back(instance);
    }

    instanceBuffer.Copy(instances.data());

    const auto instancesData = vk::AccelerationStructureGeometryInstancesDataKHR()
        .setArrayOfPointers(false)
        .setData(instanceBuffer.GetAddress());

    geometry.Update(instancesData, primitiveCount);
    const auto size = geometry.GetAccelSize();
    const auto type = vk::AccelerationStructureTypeKHR::eTopLevel;

    BuildAccel(*accel, size, primitiveCount, geometry.GetInfo());
}
