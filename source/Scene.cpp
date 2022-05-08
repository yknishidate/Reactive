#include "Vulkan.hpp"
#include "Scene.hpp"
#include "Loader.hpp"
#include "Window.hpp"

static const float distanceEyes = 0.1;

struct BufferAddress
{
    vk::DeviceAddress vertices;
    vk::DeviceAddress indices;
    vk::DeviceAddress objects;
};

Scene::Scene(const std::string& filepath)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Loader::LoadFromFile(filepath, vertices, indices);

    meshes[0] = std::make_shared<Mesh>(vertices, indices);
    meshes[1] = std::make_shared<Mesh>(vertices, indices);
    objects[0].Init(meshes[0]);
    objects[1].Init(meshes[1]);

    //// default
    //objectsStereo.resize(1);
    //objectsStereo[0].Init(meshStereo);

    //// ray align
    //std::vector<Vertex> vertices = verticesStereo;
    //std::vector<uint32_t> indices = indicesStereo;
    //vertices.resize(vertices.size() * 2);
    //indices.resize(indices.size() * 2);
    //for (int i = 0; i < verticesStereo.size(); i++) {
    //    vertices[verticesStereo.size() + i] = verticesStereo[i];
    //}
    //for (int i = 0; i < indicesStereo.size(); i++) {
    //    indices[indicesStereo.size() + i] = indicesStereo[i];
    //}
    //mesh = std::make_shared<Mesh>(vertices, indices);

    //std::vector<Vertex> vertices;
    //std::vector<uint32_t> indices;
    //objects.resize(1);


    //for (int i = 0; i < meshes.size(); i++) {
    //    objects[i].Init(meshes[i]);
    //}
    //for (int i = 0; i < meshes.size(); i++) {
    //    objects[i + meshes.size()].Init(meshes[i]);
    //    objects[i + meshes.size()].GetTransform().Position += camera.GetRight() * distanceEyes;
    //}
}

void Scene::Setup(int width, int height)
{
    topAccels[0].Init(objects[0], vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation);
    topAccels[1].Init(objects[1], vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation);

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
    BufferAddress address;
    address.vertices = objects[0].GetMesh().GetVertexBufferAddress();
    address.indices = objects[0].GetMesh().GetIndexBufferAddress();
    address.objects = objectBuffer.GetAddress();
    addressBuffer.InitOnHost(sizeof(BufferAddress), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
    addressBuffer.Copy(&address);

    camera.SetViewSize(width, height);
}

void Scene::Update(float dt)
{
    static float time = 0.0f;
    time += dt;
    objects[0].GetTransform().Position = camera.GetRight() * distanceEyes;
    objectBuffer.Copy(objectData.data());
    topAccels[0].Rebuild({ objects[0] });
}

void Scene::ProcessInput()
{
    camera.ProcessInput();
}
