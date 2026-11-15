#pragma once

/// @file Screen.hpp
/// @brief Read-only access to the window's size, from anywhere in game code.

#include <forge2d/graphics/FrameInfo.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    struct FrameInfo;

    /// @brief The size of the game window, readable from anywhere in game code.
    ///
    /// `Screen` only has static functions, so there's nothing to create or pass around:
    /// call `forge::Screen::width()` from a `Game`, a player, a UI widget or any other code.
    ///
    /// ### Units
    ///
    /// Sizes are in **logical (screen-space) units**, the same units `Renderer2D` draws in when
    /// no camera is set, and the same units as `InputManager::mousePosition()`. The origin is
    /// the window's top-left corner and +Y points down, so `size()` is also the bottom-right
    /// corner. On high-DPI displays a logical unit can cover more than one pixel (see
    /// `FrameInfo`).
    ///
    /// These are **not** world units: once `Renderer2D::setCamera()` is used, how much of the
    /// world fits on screen also depends on the camera's position and zoom.
    ///
    /// ### When values change
    ///
    /// `Application` samples the size once per frame, after processing window events and before
    /// `Game::update()`. Every call during the same frame, in `update()` and `render()` alike,
    /// returns the same size, even while the window is being resized. A resize shows up in the
    /// next frame.
    ///
    /// The size is first sampled when the `Application` is created, so it's already valid in
    /// `Game::onStart()`.
    ///
    /// ### Lifetime
    ///
    /// `Screen` works while an `Application` exists, which covers every `Game` function and the
    /// game's destructor.
    ///
    /// ### Threading
    ///
    /// Not thread-safe. Call it from the main thread.
    ///
    /// @code
    /// void update(float deltaTime, const forge::InputManager& input) override
    /// {
    ///     // Stop the player at the right edge of the window.
    ///     const float rightEdge = forge::Screen::width() - player_.width;
    ///     if (player_.x > rightEdge) player_.x = rightEdge;
    /// }
    ///
    /// void render(forge::Renderer2D& renderer) override
    /// {
    ///     if (forge::Screen::isMinimised()) return;   // nothing will be drawn anyway
    ///
    ///     // A 200×20 health bar centred near the top of the window.
    ///     const forge::math::Vec2 barSize { 200.0f, 20.0f };
    ///     renderer.drawRect({
    ///             .position = { forge::Screen::centre().x - barSize.x / 2.0f, 16.0f },
    ///             .scale    = barSize
    ///     }, forge::colours::Red);
    /// }
    /// @endcode
    ///
    /// @pre An `Application` exists. Calling a function before one is created, or after it's
    ///      destroyed, asserts in Debug builds. In other builds the sizes are `0` and
    ///      `isMinimised()` returns `true`.
    class Screen
    {
        public:
            /// @brief Deleted: @p Screen only has static functions and can't be created.
            Screen() = delete;   // static-only

            /// @brief The window's width in logical units.
            ///
            /// The same as `size().x`. `0` while the window is minimised.
            [[nodiscard]] static float      width()  noexcept;

            /// @brief The window's height in logical units.
            ///
            /// The same as `size().y`. `0` while the window is minimised.
            [[nodiscard]] static float      height() noexcept;

            /// @brief The window's width and height in logical units.
            ///
            /// Because the origin is the top-left corner, this is also the position of the
            /// bottom-right corner in screen space.
            [[nodiscard]] static math::Vec2 size()   noexcept;

            /// @brief The centre of the window in screen space: `size() / 2`.
            ///
            /// Handy for centring menus, titles and HUD elements. It's the middle of the
            /// window, not of the world: with a camera set, the world point at the centre of the
            /// screen is `Camera2D::position()`.
            [[nodiscard]] static math::Vec2 centre() noexcept;

            /// @brief Whether the window currently has no drawable area, for example because it
            ///        is minimised.
            ///
            /// While this is `true`, `width()` and `height()` may be `0` and the renderer skips
            /// the frame, so `Game::render()` can return early. Game logic in `Game::update()`
            /// keeps running; check this if the game should pause while minimised.
            ///
            /// @return `true` if the framebuffer's width or height is `0`.
            [[nodiscard]] static bool       isMinimised() noexcept;

        private:
            friend class Application;

            // Engine-only: Application calls these to feed Screen its values.

            /// @brief Starts serving @p initial. Called when the `Application` is created.
            static void attach(const FrameInfo& initial) noexcept;

            /// @brief Replaces the size with this frame's. Called once per frame, before
            ///        @p Game::update().
            static void update(const FrameInfo& frame)   noexcept;

            /// @brief Stops serving values and resets them to @p 0 . Called when the
            ///        `Application` is destroyed, after the game.
            static void detach()                         noexcept;
    };
}
