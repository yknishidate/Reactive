#pragma once
#include <memory>
#include "Vulkan.hpp"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan_hpp.h>

#include "Image.hpp"
#include "Pipeline.hpp"
#include "Window.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"

struct PushConstants
{
    glm::mat4 InvView{ 1 };
    glm::mat4 InvProj{ 1 };
    int Frame = 0;
    int NumFaces = 0;
    int Left = 1;
};

class Engine
{
public:
    void Init();
    void Run();

private:
    Image inputImage;
    Image outputImage;
    RayTracingPipeline rtPipeline;
    RayTracingPipeline stereoPipeline;
    PushConstants pushConstants;
    std::unique_ptr<Scene> scene;
    bool useRayAlign = false;
};
