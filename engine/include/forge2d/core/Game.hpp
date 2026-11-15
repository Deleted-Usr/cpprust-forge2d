#pragma once

/// @file Game.hpp
/// @brief The interface a game implements to be run by `Application`.

#include <forge2d/assets/AssetManager.hpp>
#include <forge2d/input/InputManager.hpp>

namespace forge
{
    class Renderer2D;

    /// @brief Base class for a game. Override its functions and pass it to `Application`.
    ///
    /// `Application` calls the functions in this order:
    /// 1. `onStart()` once, before the first frame;
    /// 2. `update()` then `render()`, once each per frame, until the window closes;
    /// 3. `onStop()` once, after the last frame.
    ///
    /// All of them are called on the main thread.
    ///
    /// @code
    /// class MyGame final : public forge::Game
    /// {
    ///     public:
    ///         void onStart(forge::AssetManager& assets) override
    ///         {
    ///             heart_ = assets.loadTexture("heart16x16.png");
    ///         }
    ///
    ///         void update(float deltaTime, const forge::InputManager& input) override
    ///         {
    ///             if (input.isKeyDown(forge::Key::D)) x_ += 200.0f * deltaTime;
    ///         }
    ///
    ///         void render(forge::Renderer2D& renderer) override
    ///         {
    ///             renderer.drawSprite(heart_, { .position = { x_, 100 }, .scale = { 32, 32 } });
    ///         }
    ///
    ///     private:
    ///         forge::Texture2D heart_;
    ///         float x_ = 0.0f;
    /// };
    /// @endcode
    class Game
    {
        public:
            /// @brief Destroys the game. `Application` does this before destroying the renderer,
            ///        asset manager and window, so members holding GPU resources are safe.
            virtual ~Game() = default;

            /// @brief Called once before the first frame. Load assets and set up the game here.
            ///
            /// @param assetManager The application's asset manager. It outlives the game, so
            ///                     keeping a reference or pointer to it is safe.
            virtual void onStart([[maybe_unused]] AssetManager& assetManager) {}

            /// @brief Called once after the window has closed and the last frame has finished.
            ///
            /// Not called if the game loop exits because of an exception.
            virtual void onStop()  {}

            /// @brief Advances the game by one frame.
            ///
            /// @param deltaTime Seconds since the previous frame (variable, not clamped).
            /// @param input     This frame's input snapshot. It already includes every event since
            ///                  the previous frame, and is only valid during this call.
            virtual void update(float deltaTime, const InputManager& input) = 0;

            /// @brief Draws the current frame.
            ///
            /// Called after `update()`. The renderer has already started the frame in screen
            /// space with the screen cleared; it ends the frame after this returns.
            ///
            /// @param renderer The renderer to draw with. Only valid during this call.
            virtual void render(Renderer2D& renderer) = 0;
    };
}
