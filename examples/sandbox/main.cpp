
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
            const forge::Texture2D* heart;

            void onStart(forge::AssetManager& assetManager) override
            {
                heart = &assetManager.loadTexture("heart16x16.png");
            }

            void update(float deltaTime) override
            {
                x += deltaTime;
            }

            void render(forge::Renderer2D& renderer) override
            {
                if (bool drawRect = true)
                {
                    renderer.drawRect(
                        forge::math::Transform2D {
                            .position = { 2.0f, 1.0f },
                            .rotation = 0.0f,
                            .scale    = { 2.0f, 2.0f }
                        },
                        forge::colours::Cyan
                    );
                }

                if (bool drawTriangle = false)
                {
                    renderer.drawTriangle(
                        forge::math::Transform2D {
                            .position = { 170.4f, 0.0f },
                            .rotation = 0.0f,
                            .scale    = { 100.0f, 100.0f }
                        },
                        forge::colours::Red
                    );
                }

                if (bool drawSprite = true)
                {
                    renderer.drawSprite(
                        *heart,
                        forge::math::Transform2D {
                            .position = { 100.f, 150.f },
                            .scale    = { 128.0f, 128.0f }
                        }
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
        config.window.resizable = false;

        config.window.width = 800;
        config.window.height = 600;

        config.assetRoot = "assets";

        forge::Application app(config, std::make_unique<SandboxGame>());
        return app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }
}
