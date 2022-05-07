#include "Vulkan.hpp"
#include "Scene.hpp"
#include "Loader.hpp"
#include "Window.hpp"

struct BufferAddress
{
    vk::DeviceAddress vertices;
    vk::DeviceAddress indices;
    vk::DeviceAddress objects;
};

Scene::Scene(const std::string& filepath)
{
    Loader::LoadFromFile(filepath, meshes, textures);

    objects.resize(meshes.size() * 2);
    for (int i = 0; i < meshes.size(); i++) {
        objects[i].Init(meshes[i]);
    }
    for (int i = 0; i < meshes.size(); i++) {
        objects[i + meshes.size()].Init(meshes[i]);
        objects[i + meshes.size()].GetTransform().Position += camera.GetRight();
    }
}

void Scene::Setup()
{
    topAccel.Init(objects, vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation);

    // Create object data
    for (auto&& object : objects) {
        glm::mat4 matrix = object.GetTransform().GetMatrix();
        glm::mat4 normalMatrix = object.GetTransform().GetNormalMatrix();
        glm::vec3 diffuse = object.GetMesh().GetMaterial().Diffuse;
        int texIndex = object.GetMesh().GetMaterial().DiffuseTexture;
        objectData.push_back({ matrix, normalMatrix, glm::vec4(diffuse, 1), texIndex });
    }
    objectBuffer.InitOnHost(sizeof(ObjectData) * objectData.size(),
                            vk::BufferUsageFlagBits::eStorageBuffer |
                            vk::BufferUsageFlagBits::eShaderDeviceAddress);
    objectBuffer.Copy(objectData.data());

    // Buffer references
    std::vector<BufferAddress> addresses(objects.size());
    for (int i = 0; i < objects.size(); i++) {
        addresses[i].vertices = objects[i].GetMesh().GetVertexBufferAddress();
        addresses[i].indices = objects[i].GetMesh().GetIndexBufferAddress();
        addresses[i].objects = objectBuffer.GetAddress();
    }
    addressBuffer.InitOnHost(sizeof(BufferAddress) * addresses.size(), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
    addressBuffer.Copy(addresses.data());

    camera.Init(Window::GetWidth(), Window::GetHeight());
}

void Scene::Update(float dt)
{
    static float time = 0.0f;
    time += dt;
    for (int i = 0; i < meshes.size(); i++) {
        objects[i + meshes.size()].GetTransform().Position = camera.GetRight();
    }
    objectBuffer.Copy(objectData.data());
    topAccel.Rebuild(objects);
}

void Scene::ProcessInput()
{
    camera.ProcessInput();
}

const std::shared_ptr<Mesh>& Scene::AddMesh(const std::string& filepath)
{
    meshes.push_back(std::make_shared<Mesh>(filepath));
    return meshes.back();
}

const Object& Scene::AddObject(std::shared_ptr<Mesh> mesh)
{
    Object object;
    object.Init(mesh);
    objects.push_back(object);
    return objects.back();
}
