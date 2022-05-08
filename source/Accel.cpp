#include "Vulkan.hpp"
#include "Accel.hpp"
#include "Object.hpp"

namespace
{
    vk::UniqueAccelerationStructureKHR CreateAccel(vk::Buffer buffer, vk::DeviceSize size,
                                                   vk::AccelerationStructureTypeKHR type)
    {
        vk::AccelerationStructureCreateInfoKHR createInfo;
        createInfo.setBuffer(buffer);
        createInfo.setSize(size);
        createInfo.setType(type);
        return Vulkan::Device.createAccelerationStructureKHRUnique(createInfo);
    }

    vk::DeviceSize GetAccelSize(vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo, size_t primitiveCount)
    {
        auto buildSizes = Vulkan::Device.getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, geometryInfo, primitiveCount);
        return buildSizes.accelerationStructureSize;
    }

    Buffer CreateAccelBuffer(vk::DeviceSize size,
                             vk::AccelerationStructureTypeKHR type,
                             vk::AccelerationStructureGeometryKHR geometry)
    {
        Buffer buffer;
        buffer.InitOnDevice(size,
                            vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
                            vk::BufferUsageFlagBits::eShaderDeviceAddress);
        return buffer;
    }

    void BuildAccel(vk::AccelerationStructureKHR accel, vk::DeviceSize size, uint32_t primitiveCount,
                    vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo)
    {
        Buffer scratchBuffer;
        scratchBuffer.InitOnDevice(size, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);

        geometryInfo.setScratchData(scratchBuffer.GetAddress());
        geometryInfo.setDstAccelerationStructure(accel);

        Vulkan::OneTimeSubmit(
            [&](vk::CommandBuffer commandBuffer)
            {
                vk::AccelerationStructureBuildRangeInfoKHR buildRangeInfo{ primitiveCount, 0, 0, 0 };
                commandBuffer.buildAccelerationStructuresKHR(geometryInfo, &buildRangeInfo);
            });
    }
}

void BottomAccel::Init(const Buffer& vertexBuffer, const Buffer& indexBuffer,
                       size_t vertexCount, size_t primitiveCount,
                       vk::GeometryFlagBitsKHR geomertyFlag)
{
    this->primitiveCount = primitiveCount;
    triangleData.setVertexFormat(vk::Format::eR32G32B32Sfloat);
    triangleData.setVertexData(vertexBuffer.GetAddress());
    triangleData.setVertexStride(sizeof(Vertex));
    triangleData.setMaxVertex(vertexCount);
    triangleData.setIndexType(vk::IndexType::eUint32);
    triangleData.setIndexData(indexBuffer.GetAddress());

    geometry.setGeometryType(vk::GeometryTypeKHR::eTriangles);
    geometry.setGeometry({ triangleData });
    geometry.setFlags(geomertyFlag);


    geometryInfo.setType(type);
    geometryInfo.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
    geometryInfo.setGeometries(geometry);

    size = GetAccelSize(geometryInfo, primitiveCount);
    buffer = CreateAccelBuffer(size, type, geometry);
    accel = CreateAccel(buffer.GetBuffer(), size, type);
    BuildAccel(*accel, size, primitiveCount, geometryInfo);
}

void BottomAccel::Rebuild() const
{
    BuildAccel(*accel, size, primitiveCount, geometryInfo);
}

void TopAccel::Init(const Object& object, vk::GeometryFlagBitsKHR geomertyFlag)
{
    this->geomertyFlag = geomertyFlag;
    uint32_t primitiveCount = 1;

    vk::AccelerationStructureInstanceKHR instance;
    instance.setTransform(object.GetTransform().GetVkMatrix());
    instance.setMask(0xFF);
    instance.setAccelerationStructureReference(object.GetMesh().GetAccel().GetBufferAddress());
    instance.setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable);

    instanceBuffer.InitOnHost(sizeof(vk::AccelerationStructureInstanceKHR) * primitiveCount,
                              vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
                              vk::BufferUsageFlagBits::eShaderDeviceAddress);
    instanceBuffer.Copy(&instance);

    vk::AccelerationStructureGeometryInstancesDataKHR instancesData;
    instancesData.setArrayOfPointers(false);
    instancesData.setData(instanceBuffer.GetAddress());

    vk::AccelerationStructureGeometryKHR geometry;
    geometry.setGeometryType(vk::GeometryTypeKHR::eInstances);
    geometry.setGeometry({ instancesData });
    geometry.setFlags(geomertyFlag);

    vk::AccelerationStructureTypeKHR type = vk::AccelerationStructureTypeKHR::eTopLevel;

    vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo;
    geometryInfo.setType(type);
    geometryInfo.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
    geometryInfo.setGeometries(geometry);

    vk::DeviceSize size = GetAccelSize(geometryInfo, primitiveCount);
    buffer = CreateAccelBuffer(size, type, geometry);
    accel = CreateAccel(buffer.GetBuffer(), size, type);
    BuildAccel(*accel, size, primitiveCount, geometryInfo);
}

void TopAccel::Rebuild(const std::vector<Object>& objects)
{
    uint32_t primitiveCount = objects.size();

    std::vector<vk::AccelerationStructureInstanceKHR> instances;
    for (auto&& object : objects) {
        vk::AccelerationStructureInstanceKHR instance;
        instance.setTransform(object.GetTransform().GetVkMatrix());
        instance.setMask(0xFF);
        instance.setAccelerationStructureReference(object.GetMesh().GetAccel().GetBufferAddress());
        instance.setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable);
        instances.push_back(instance);
    }

    instanceBuffer.Copy(instances.data());

    vk::AccelerationStructureGeometryInstancesDataKHR instancesData;
    instancesData.setArrayOfPointers(false);
    instancesData.setData(instanceBuffer.GetAddress());

    vk::AccelerationStructureGeometryKHR geometry;
    geometry.setGeometryType(vk::GeometryTypeKHR::eInstances);
    geometry.setGeometry({ instancesData });
    geometry.setFlags(geomertyFlag);

    vk::AccelerationStructureTypeKHR type = vk::AccelerationStructureTypeKHR::eTopLevel;

    vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo;
    geometryInfo.setType(type);
    geometryInfo.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
    geometryInfo.setGeometries(geometry);

    vk::DeviceSize size = GetAccelSize(geometryInfo, primitiveCount);
    BuildAccel(*accel, size, primitiveCount, geometryInfo);
}
