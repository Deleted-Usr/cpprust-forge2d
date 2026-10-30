
#include <forge2d/core/Application.hpp>

#include <exception>
#include <iostream>
#include <memory>

namespace
{
    class SandboxGame final : public forge::Game
    {
        public:
            float x = 0, y = 0;

            void update(float deltaTime) override
            {

            }

            void render(forge::Renderer2D& renderer) override
            {
                if (bool drawRect = true)
                {
                    renderer.drawRect(
                        forge::math::Transform2D {
                            .position = { x, 0.0f },
                            .rotation = 0.0f,
                            .scale = { 1.0f, 1.0f }
                        },
                        forge::colours::Cyan
                    );
                }

                if (bool drawTriangle = false)
                {
                    renderer.drawTriangle(
                        forge::math::Transform2D {
                            .position = { 0.0f, 0.0f },
                            .rotation = 0.0f,
                            .scale = { 1.0f, 1.0f }
                        },
                        forge::colours::Cyan
                    );
                }
            }
    };
}

int main()
{
    try
    {
        forge::ApplicationConfig config;

        config.window.title = "Forge2D Sandbox";
        config.window.width = 800;
        config.window.height = 600;

        forge::Application app(config, std::make_unique<SandboxGame>());
        return app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }
}
