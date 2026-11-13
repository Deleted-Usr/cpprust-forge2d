#include <forge2d/core/Application.hpp>

#include <chrono>
#include <utility>

namespace forge
{
    Application::Application(
        const ApplicationConfig& config,
        std::unique_ptr<Game> game
    ) : window_(config.window), assets_(config.assetRoot), renderer_("shaders"), game_(std::move(game))
    { }

    int Application::run()
    {
        using Clock = std::chrono::steady_clock;

        auto previousTime = Clock::now();

        game_->onStart(assets_);

        while (!window_.shouldClose())
        {
            // Time
            const auto currentTime = Clock::now();
            const std::chrono::duration<float> elapsed = currentTime - previousTime;

            previousTime = currentTime;

            const float deltaTime = elapsed.count();

            // Events
            window_.pollEvents();
            window_.updateInput(input_);

            // Update
            game_->update(deltaTime, input_);

            // Render
            renderer_.beginFrame(window_.frameInfo(), {
                0.08f,
                0.08f,
                0.10f,
                1.0f
            });

            game_->render(renderer_);

            renderer_.endFrame();

            // Present
            window_.swapBuffers();
        }

        game_->onStop();

        return 0;
    }
}
