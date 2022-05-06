#pragma once
#include <vector>
#include <memory>
#include "Mesh.hpp"
#include "Camera.hpp"

//class Image;
//class Object;
//
//struct ObjectData
//{
//    glm::mat4 Matrix{ 1 };
//    glm::mat4 NormalMatrix{ 1 };
//    glm::vec4 Diffuse{ 1.0 };
//    int TextureIndex = -1;
//};
//
//class Scene
//{
//public:
//    explicit Scene(const std::string& filepath);
//
//    vk::AccelerationStructureKHR GetAccel() const { return topAccel.GetAccel(); }
//    const std::vector<Image>& GetTextures() const { return textures; }
//    const Buffer& GetAddressBuffer() const { return addressBuffer; }
//
//private:
//    std::vector<std::shared_ptr<Mesh>> meshes;
//    std::vector<Image> textures;
//
//    std::vector<Object> objects;
//    std::vector<ObjectData> objectData;
//
//    Accel topAccel;
//    Camera camera;
//
//    Buffer objectBuffer;
//    Buffer addressBuffer;
//};
