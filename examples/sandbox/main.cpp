
#include <forge2d/core/Application.hpp>

#include <exception>
#include <iostream>
#include <memory>

namespace
{
    class SandboxGame final : public forge::Game
    {
        public:
            void update(float) override {}

            void render(forge::Renderer2D& renderer) override
            {
                renderer.drawRect(
                    forge::math::Transform2D {
                        .position = { 0.0f, 0.0f },
                        .rotation = 0.0f,
                        .scale = { 1.0f, 1.0f }
                    },
                    forge::colours::Cyan
                );
            }
    };
}

int main()
{
    try
    {
        forge::ApplicationConfig config;
        config.window.title = "Forge2D Sandbox";

        forge::Application app(config, std::make_unique<SandboxGame>());
        return app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }
}
