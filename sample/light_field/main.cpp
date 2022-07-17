#include "Engine.hpp"

struct PushConstants
{
    int rows = 17;
    int cols = 17;
    glm::vec2 st = { 0.5, 0.5 };

    void HandleInput(bool invX, bool invY)
    {
        if (!Window::MousePressed()) return;
        auto motion = Window::GetMouseMotion() * 0.005f;
        if (invX) motion.x = -motion.x;
        if (invY) motion.y = -motion.y;
        st = clamp(st + glm::vec2(motion.x, motion.y), 0.0f, 0.99f);
    }
};

namespace fs = std::filesystem;
std::vector<std::string> GetAllFilePaths(const fs::path& directory)
{
    std::vector<std::string> imagePaths;
    for (const auto& file : fs::directory_iterator{ directory }) {
        imagePaths.push_back(file.path().string());
    }
    return imagePaths;
}

int main()
{
    Engine::Init(1280, 1280);

    auto imagePaths = GetAllFilePaths(ASSET_DIR + "lego_lf");
    Image images{ imagePaths };
    Image outputImage{ vk::Format::eB8G8R8A8Unorm };

    ComputePipeline pipeline{ };
    pipeline.LoadShaders(SHADER_DIR + "light_field.comp");
    pipeline.GetDescSet().Register("outputImage", outputImage);
    pipeline.GetDescSet().Register("images", images);
    pipeline.GetDescSet().Setup();
    pipeline.Setup(sizeof(PushConstants));

    PushConstants pushConstants;
    bool invertX = false;
    bool invertY = false;
    while (Engine::Update()) {
        GUI::Checkbox("Invert x", invertX);
        GUI::Checkbox("Invert y", invertY);

        pushConstants.HandleInput(invertX, invertY);
        Engine::Render([&]() {
            pipeline.Run(&pushConstants);
            outputImage.CopyToBackImage(); });
    }
    Engine::Shutdown();
}
