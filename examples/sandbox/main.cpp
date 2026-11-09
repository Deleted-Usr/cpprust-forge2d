#include <exception>
#include <iostream>
#include <memory>

#include <forge2d/core/Application.hpp>
#include <forge2d/debug/Log.hpp>

namespace
{
    struct Player
    {
        float x = 300.f, y = 200.f;

        void update(float deltaTime)
        {
            x += 1;
        }

        void render(forge::Renderer2D& renderer)
        {
            renderer.drawRect(
                forge::math::Transform2D {
                    .position = { x, y },
                    .scale    = { 100.0f, 100.0f }
                },
                forge::colours::Green
            );
        }

        [[nodiscard]]
        forge::math::Vec2 position() const
        {
            return { x, y };
        }
    };

    class SandboxGame final : public forge::Game
    {
        public:
            float x = 0, y = 0;
            const forge::Texture2D* heart{};

            forge::Camera2D camera;
            Player player;

            void onStart(forge::AssetManager& assetManager) override
            {
                heart = &assetManager.loadTexture("heart16x16.png");

                camera.follow(player);
            }

            void update(float deltaTime) override
            {
                player.update(deltaTime);
                camera.update(deltaTime);
            }

            void render(forge::Renderer2D& renderer) override
            {
                if (bool useCamera = false)
                {
                    renderer.setCamera(camera);
                }
                player.render(renderer);

                if (bool drawRect = true)
                {
                    renderer.drawRect(
                        forge::math::Transform2D {
                            .position = { 15.0f, 15.0f },
                            .scale    = { 100.0f, 100.0f }
                        },
                        forge::colours::Cyan
                    );
                }

                if (bool drawTriangle = true)
                {
                    renderer.drawTriangle(
                        forge::math::Transform2D {
                            .position = { 170.4f, 50.0f },
                            .scale    = { 200.0f, 200.0f }
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
                            .scale    = { 512.0f, 512.0f }
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

        config.window.title     = "Forge2D Sandbox";
        config.window.resizable = false;

        config.window.width  = 800;
        config.window.height = 600;

        config.assetRoot = "assets";

        forge::Application app(config, std::make_unique<SandboxGame>());
        return app.run();
    }
    catch (const std::exception& e)
    {
        FORGE_LOG_FATAL("Fatal Error: {}", e.what());
        return 1;
    }
}
