#pragma once
#include "Buffer.hpp"

class Accel
{
public:
    void InitAsBottom(const Buffer& vertexBuffer, const Buffer& indexBuffer,
                      size_t vertexStride, size_t vertexCount, size_t primitiveCount);

    void InitAsTop(const Accel& bottom);

    vk::AccelerationStructureKHR GetAccel() const { return *accel; }

private:
    vk::UniqueAccelerationStructureKHR accel;
    Buffer buffer;
};