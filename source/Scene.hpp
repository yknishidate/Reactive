#pragma once
#include <vector>
#include <memory>
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Object.hpp"

class Image;

struct ObjectData
{
    glm::mat4 Matrix{ 1 };
    glm::mat4 NormalMatrix{ 1 };
    glm::vec4 Diffuse{ 1.0 };
    int TextureIndex = -1;
};

class Scene
{
public:
    explicit Scene(const std::string& filepath);
    void Setup(int width, int height);
    void Update(float dt);
    void ProcessInput();

    vk::AccelerationStructureKHR GetAccel() const { return topAccel.GetAccel(); }
    vk::AccelerationStructureKHR GetAccelStereo() const { return topAccelStereo.GetAccel(); }
    const Buffer& GetAddressBuffer() const { return addressBuffer; }

    std::shared_ptr<Mesh> meshes[2];
    Object objects[2];
    std::vector<ObjectData> objectData;

    TopAccel topAccels[2];
    Camera camera;

    Buffer objectBuffer;
    Buffer addressBuffer;
};
