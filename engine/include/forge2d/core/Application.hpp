#pragma once

/// @file Application.hpp
/// @brief Owns the engine's systems and runs the game loop.

#include <memory>

#include <forge2d/core/Game.hpp>
#include <forge2d/core/Window.hpp>
#include <forge2d/input/InputManager.hpp>

#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/assets/AssetManager.hpp>
#include <forge2d/graphics/Camera2D.hpp>



namespace forge
{
    /// @brief Settings used to create an `Application`.
    ///
    /// @code
    /// forge::ApplicationConfig config {
    ///     .window    = { .title = "Platformer", .width = 1600, .height = 900 },
    ///     .assetRoot = "games/platformer/assets"
    /// };
    /// @endcode
    struct ApplicationConfig
    {
        /// Settings for the game window.
        WindowConfig window;

        /// Folder that `AssetManager` paths are relative to. A relative path is resolved against
        /// the current working directory. Defaults to `"assets"`.
        std::filesystem::path assetRoot = "assets";
    };

    /// @brief Creates the window, input, assets and renderer, then runs a `Game` on them.
    ///
    /// Each frame of `run()`:
    /// 1. measures the time since the previous frame;
    /// 2. processes window and input events, and samples the window size for `Screen`;
    /// 3. calls `Game::update()`;
    /// 4. clears the screen, calls `Game::render()` and presents the frame.
    ///
    /// The loop has a variable time step: `deltaTime` is the real time between frames and isn't
    /// clamped, so a long pause (e.g. dragging the window) produces one large step.
    ///
    /// Systems are created in the order window, input, assets, renderer, and destroyed in
    /// reverse after the game, so everything a game holds (such as `Texture2D` handles) is
    /// released while the GL context still exists.
    ///
    /// @code
    /// int main()
    /// {
    ///     forge::Application app({ .window = { .title = "My Game" } }, std::make_unique<MyGame>());
    ///     return app.run();
    /// }
    /// @endcode
    ///
    /// @note Create and run it on the main thread. Only create one at a time.
    class Application
    {
        public:
            /// @brief Opens the window and creates the engine's systems.
            ///
            /// The game isn't started until `run()`. `Screen` works from the end of this
            /// constructor until the application is destroyed.
            ///
            /// @param config Window and asset settings.
            /// @param game   The game to run. The application takes ownership.
            ///
            /// @pre @p game is not null.
            /// @throws std::runtime_error If the window or OpenGL 4.6 context can't be created, or
            ///         the renderer's shaders fail to load or compile (see `Renderer2D`).
            ///
            /// @note In Debug builds, the renderer reads its shaders from a `shaders` folder in
            ///       the current working directory.
            Application(const ApplicationConfig& config, std::unique_ptr<Game> game);

            /// @brief Destroys the game, then the engine's systems.
            ///
            /// The game is destroyed first, while `Screen`, the renderer, the asset manager and
            /// the GL context still exist, so its destructor can use them and release GPU
            /// resources safely. `Screen` is then detached, and the renderer, asset manager,
            /// input and window are destroyed in that order.
            ~Application();

            /// @brief Runs the game until the window is closed.
            ///
            /// Calls `Game::onStart()` once, then runs the game loop (see the class
            /// documentation) until the user closes the window, then calls `Game::onStop()`.
            ///
            /// @return The process exit code. Currently always `0`.
            ///
            /// @note Exceptions thrown by the game propagate out of `run()`, and `onStop()` isn't
            ///       called in that case.
            int run();

        private:
            Window       window_;
            InputManager input_;
            AssetManager assets_;
            Renderer2D   renderer_;

            std::unique_ptr<Game> game_;
    };
}
