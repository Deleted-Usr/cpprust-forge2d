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

        void update(float deltaTime, const forge::InputManager& input)
        {
            if (input.isKeyDown(forge::Key::W)) y -= 200.0f * deltaTime;
            if (input.isKeyDown(forge::Key::A)) x -= 200.0f * deltaTime;
            if (input.isKeyDown(forge::Key::S)) y += 200.0f * deltaTime;
            if (input.isKeyDown(forge::Key::D)) x += 200.0f * deltaTime;
        }

        void render(forge::Renderer2D& renderer)
        {
            renderer.drawCircle(
                { x, y },
                10.f,
                forge::colours::Cyan
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
            const forge::Font*      font{};

            forge::Camera2D camera;
            Player player;

            void onStart(forge::AssetManager& assetManager) override
            {
                heart = &assetManager.loadTexture("heart16x16.png");
                font  = &assetManager.loadFont("pixelFont.ttf");

                camera.follow(player);
            }

            void update(float deltaTime, const forge::InputManager& input) override
            {
                player.update(deltaTime, input);
                camera.update(deltaTime);
            }

            void render(forge::Renderer2D& renderer) override
            {
                if (bool useCamera = false)
                {
                    renderer.setCamera(camera);
                }
                player.render(renderer);

                renderer.drawText("This is a cool text thingy!", font, { 32.f, 100.f }, 32);

                if (bool drawRect = false)
                {
                    renderer.drawRect(
                        forge::math::Transform2D {
                            .position = { 15.0f,  15.0f  },
                            .scale    = { 100.0f, 100.0f }
                        },
                        forge::colours::Cyan,
                        false
                    );
                }

                if (bool drawTriangle = false)
                {
                    renderer.drawTriangle(
                        forge::math::Transform2D {
                            .position = { 170.4f, 50.0f  },
                            .scale    = { 200.0f, 200.0f }
                        },
                        forge::colours::Red
                    );
                }

                if (bool drawSprite = false)
                {
                    renderer.drawSprite(
                        *heart,
                        forge::math::Transform2D {
                            .position = { 100.f,  150.f },
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

        config.window.clearColour = {
            0.08f,
            0.08f,
            0.10f,
            1.0f
        };

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
