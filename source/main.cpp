#include "Vulkan.hpp"
#include "Engine.hpp"

int main()
{
    try {
        spdlog::set_pattern("[%^%l%$] %v");

        int input;
        while (1) {
            Window::Init(2048, 1024, "../asset/Vulkan.png");
            Vulkan::Init();
            Window::SetupUI();
            std::cin >> input;
            {
                Engine engine;
                engine.Init(input);
                engine.Run();
            }
            Window::Shutdown();
            Vulkan::Shutdown();
        }

    } catch (const std::exception& exception) {
        spdlog::error(exception.what());
    }

    return 0;
}
