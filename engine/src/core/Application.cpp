#include <forge2d/core/Application.hpp>

#include <chrono>
#include <utility>

namespace forge
{
    Application::Application(
        const ApplicationConfig& config,
        std::unique_ptr<Game> game
    ) : window_(config.window), renderer_("shaders"), game_(std::move(game))
    { }

    int Application::run()
    {
        using Clock = std::chrono::steady_clock;

        auto previousTime = Clock::now();

        game_->onStart();

        while (!window_.shouldClose())
        {
            // Time
            const auto currentTime = Clock::now();
            const std::chrono::duration<float> elapsed = currentTime - previousTime;

            previousTime = currentTime;

            const float deltaTime = elapsed.count();

            // Events
            window_.pollEvents();

            // Update
            game_->update(deltaTime);

            // Render
            renderer_.beginFrame({
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
