#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan_hpp.h>
#include "UI.hpp"
#include "Window.hpp"

void UI::Init()
{
    spdlog::info("UI::Init()");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    // Color scheme
    ImVec4 red80 = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);
    ImVec4 red60 = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
    ImVec4 red40 = ImVec4(0.4f, 0.0f, 0.0f, 1.0f);
    ImVec4 black100 = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 black90 = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    ImVec4 black80 = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 black60 = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = black90;
    style.Colors[ImGuiCol_TitleBg] = red80;
    style.Colors[ImGuiCol_TitleBgActive] = red80;
    style.Colors[ImGuiCol_MenuBarBg] = red40;
    style.Colors[ImGuiCol_Header] = red40;
    style.Colors[ImGuiCol_HeaderActive] = red40;
    style.Colors[ImGuiCol_HeaderHovered] = red40;
    style.Colors[ImGuiCol_FrameBg] = black100;
    style.Colors[ImGuiCol_FrameBgHovered] = black80;
    style.Colors[ImGuiCol_FrameBgActive] = black60;
    style.Colors[ImGuiCol_CheckMark] = red80;
    style.Colors[ImGuiCol_SliderGrab] = red40;
    style.Colors[ImGuiCol_SliderGrabActive] = red80;
    style.Colors[ImGuiCol_Button] = red40;
    style.Colors[ImGuiCol_ButtonHovered] = red60;
    style.Colors[ImGuiCol_ButtonActive] = red80;
    style.Colors[ImGuiCol_ResizeGrip] = red40;
    style.Colors[ImGuiCol_ResizeGripHovered] = red60;
    style.Colors[ImGuiCol_ResizeGripActive] = red80;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(Window::GetWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = Vulkan::GetInstance();
    initInfo.PhysicalDevice = Vulkan::GetPhysicalDevice();
    initInfo.Device = Vulkan::GetDevice();
    initInfo.QueueFamily = Vulkan::GetQueueFamily();
    initInfo.Queue = Vulkan::GetQueue();
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = Vulkan::GetDescriptorPool();
    initInfo.Subpass = 0;
    initInfo.MinImageCount = Vulkan::GetMinImageCount();
    initInfo.ImageCount = Vulkan::GetImageCount();
    initInfo.MSAASamples = vk::SampleCountFlagBits::e1;
    initInfo.Allocator = nullptr;
    ImGui_ImplVulkan_Init(&initInfo, Vulkan::GetRenderPass());

    // Setup font
    io.Fonts->AddFontFromFileTTF("../asset/Roboto-Medium.ttf", 24.0f);
    {
        Vulkan::OneTimeSubmit(
            [&](vk::CommandBuffer commandBuffer)
            {
                ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
            }
        );
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void UI::Shutdown()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::StartFrame()
{
    //if (swapchainRebuild) {
    //    RebuildSwapchain();
    //}
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::Prepare()
{
    ImGui::Render();
}

void UI::Render(vk::CommandBuffer commandBuffer)
{
    Vulkan::BeginRenderPass();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    Vulkan::EndRenderPass();
}

bool UI::Checkbox(const std::string& label, bool& value)
{
    return ImGui::Checkbox(label.c_str(), &value);
}

bool UI::Combo(const std::string& label, int& value, const std::vector<std::string>& items)
{
    std::string concats;
    for (auto&& item : items) {
        concats += item + '\0';
    }
    return ImGui::Combo(label.c_str(), &value, concats.c_str());
}

bool UI::SliderInt(const std::string& label, int& value, int min, int max)
{
    return ImGui::SliderInt(label.c_str(), &value, min, max);
}

bool UI::ColorPicker4(const std::string& label, glm::vec4& value)
{
    return ImGui::ColorPicker4(label.c_str(), reinterpret_cast<float*>(&value));
}
