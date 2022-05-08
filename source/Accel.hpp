#pragma once
#include "Buffer.hpp"

class Object;

class Accel
{
public:
    vk::AccelerationStructureKHR GetAccel() const { return *accel; }
    uint64_t GetBufferAddress() const { return buffer.GetAddress(); }

protected:
    vk::UniqueAccelerationStructureKHR accel;
    Buffer buffer;
};

class BottomAccel : public Accel
{
public:
    void Init(const Buffer& vertexBuffer, const Buffer& indexBuffer,
              size_t vertexCount, size_t primitiveCount, vk::GeometryFlagBitsKHR geomertyFlag);
    void Rebuild() const;
private:
    vk::AccelerationStructureGeometryTrianglesDataKHR triangleData;
    vk::AccelerationStructureGeometryKHR geometry;
    vk::AccelerationStructureTypeKHR type = vk::AccelerationStructureTypeKHR::eBottomLevel;
    vk::AccelerationStructureBuildGeometryInfoKHR geometryInfo;
    vk::DeviceSize size;
    size_t primitiveCount;
};

class TopAccel : public Accel
{
public:
    void Init(const Object& object, vk::GeometryFlagBitsKHR geomertyFlag);
    void Rebuild(const std::vector<Object>& objects);

private:
    Buffer instanceBuffer;
    vk::GeometryFlagBitsKHR geomertyFlag;
};
