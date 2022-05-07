#include "Vulkan.hpp"
#include "Engine.hpp"

int main()
{
    try {
        spdlog::set_pattern("[%^%l%$] %v");

        Window::Init(2048, 1024, "../asset/Vulkan.png");
        Vulkan::Init();
        Window::SetupUI();

        {
            Engine engine;
            engine.Init();
            engine.Run();
        }

        Window::Shutdown();
        Vulkan::Shutdown();
    } catch (const std::exception& exception) {
        spdlog::error(exception.what());
    }

    return 0;
}
