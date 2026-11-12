#pragma once

/// @file Camera2D.hpp
/// @brief A 2D camera with following, smoothing, look-ahead, zoom, rotation and screen shake.

#include <concepts>
#include <functional>
#include <random>

#include <forge2d/math/Mat4.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// @brief Anything with a `position()` function returning something convertible to
    ///        `math::Vec2` can be followed by a `Camera2D`.
    ///
    /// @code
    /// struct Player
    /// {
    ///     math::Vec2 position() const { return pos; }
    ///     math::Vec2 pos;
    /// };
    /// static_assert(forge::Followable<Player>);
    /// @endcode
    template<typename T>
    concept Followable = requires(const T& t) { { t.position() } -> std::convertible_to<math::Vec2>; };

    /// @brief A 2D camera that decides which part of the world is on screen.
    ///
    /// `position()` is the world point shown at the **centre** of the screen.
    /// World space uses the same axes as the screen: +X right, +Y down.
    ///
    /// The camera is owned by game code, not the engine. Each frame:
    /// 1. call `update()` in `Game::update()`;
    /// 2. pass the camera to `Renderer2D::setCamera()` in `Game::render()`.
    ///
    /// @code
    /// void onStart(forge::AssetManager&) override
    /// {
    ///     camera_.follow(player_);
    ///     camera_.setSmoothing(0.15f);
    ///     camera_.setLookAhead({ 48.0f, -16.0f });
    /// }
    ///
    /// void update(float dt) override
    /// {
    ///     player_.update(dt);
    ///     camera_.update(dt);
    /// }
    ///
    /// void render(forge::Renderer2D& renderer) override
    /// {
    ///     renderer.setCamera(camera_);
    ///     // ... draw the world ...
    /// }
    /// @endcode
    ///
    /// @note Not thread-safe. Use it from the main thread.
    class Camera2D
    {
        public:
            // ---- Configuration ------------------------------------------------

            /// @brief Moves the camera to @p position immediately.
            ///
            /// While the camera is following a target, the next `update()` moves it back
            /// towards the target, so this is mainly for cameras that aren't following anything.
            ///
            /// @param position World point to show at the centre of the screen.
            void setPosition  (math::Vec2 position);

            /// @brief Sets the zoom level.
            ///
            /// @param zoom `1` draws the world at its normal size, `2` draws everything twice as
            ///             big (showing half as much of the world), `0.5` draws it half as big.
            ///             Zero, negative or non-finite values are ignored and logged once.
            void setZoom      (float zoom);

            /// @brief Sets the camera's rotation.
            ///
            /// @param radians Rotation of the camera. Rotating the camera clockwise makes the world
            ///                appear to turn counter-clockwise on screen.
            void setRotation  (float radians);

            /// @brief Sets how smoothly the camera catches up with its target.
            ///
            /// The camera eases towards its goal exponentially, so the result is the same at
            /// any frame rate. After @p seconds it has closed about 63% of the remaining distance.
            ///
            /// @param seconds The smoothing time constant. `0` (the default) locks the camera
            ///                rigidly to the target. Negative values are treated as `0`, and
            ///                non-finite values are ignored.
            void setSmoothing (float seconds);

            /// @brief Offsets the camera ahead of the target, so the player sees more of where
            ///        they're going.
            ///
            /// @param offset World-space offset from the target. The X part flips with the
            ///               direction the target is moving horizontally (it keeps its last
            ///               direction while the target is still, so it doesn't jitter). The Y
            ///               part is always applied as-is.
            void setLookAhead (math::Vec2 offset);

            // ---- Following ----------------------------------------------------

            /// @brief Follows @p target, which is captured **by reference**.
            ///
            /// The camera reads `target.position()` every `update()`. Starting to follow a new
            /// target doesn't count the jump between targets as movement, so the look-ahead
            /// direction isn't disturbed.
            ///
            /// @param target The object to follow.
            ///
            /// @warning @p target must outlive the follow. Call `stopFollowing()` or `follow()`
            ///          something else before it's destroyed or moved.
            template<Followable T>
            void follow(const T& target)
            {
                setTarget([&target] { return math::Vec2(target.position()); });
            }

            /// @brief Deleted: following a temporary would leave a dangling reference immediately,
            ///        so it's a compile error.
            template<Followable T>
            void follow(const T&&) = delete;

            /// @brief Follows a position calculated by a function, for targets without a
            ///        `position()`.
            ///
            /// @param targetPosition Called once per `update()` to get the point to follow.
            ///
            /// @code
            /// // Keep two players in view by following their midpoint.
            /// camera.follow([&] { return (p1.position() + p2.position()) * 0.5f; });
            /// @endcode
            ///
            /// @warning Anything the function captures by reference must outlive the follow.
            void follow(std::function<math::Vec2()> targetPosition)
            {
                setTarget(std::move(targetPosition));
            }

            /// @brief Stops following. The camera stays where it is.
            void stopFollowing();

            /// @brief Skips smoothing for the next `update()`, so the camera jumps straight to
            ///        its target.
            ///
            /// Use it after teleports, respawns or level loads, so the camera doesn't sweep across
            /// the level.
            void snapToTarget() noexcept { snapNext_ = true; }

            // ---- Effects ------------------------------------------------------

            /// @brief Shakes the camera.
            ///
            /// The shake moves the view randomly each `update()` and fades out quadratically over
            /// @p duration. It only affects what's drawn: `position()` is unchanged.
            ///
            /// A new shake that is weaker than the one currently running is ignored, so a small
            /// hit never cuts off a big explosion.
            ///
            /// @param strength Largest offset, in world units. Negative or non-finite values are
            ///                 ignored.
            /// @param duration Length in seconds. Zero, negative or non-finite values are ignored.
            void shake(float strength, float duration);

            // ---- Per frame ----------------------------------------------------

            /// @brief Advances following, smoothing and shake by one frame.
            ///
            /// Call once per frame from `Game::update()`, before the camera is used for drawing.
            ///
            /// @param deltaTime Seconds since the last update. Negative or non-finite values are
            ///                  ignored (the camera doesn't change).
            void update(float deltaTime);

            // ---- Queries ------------------------------------------------------

            /// @brief The world point at the centre of the screen, not including shake.
            [[nodiscard]] math::Vec2 position() const noexcept { return position_; }

            /// @brief The current zoom level. See `setZoom()`.
            [[nodiscard]] float      zoom()     const noexcept { return zoom_; }

            /// @brief The current rotation, in radians. See `setRotation()`.
            [[nodiscard]] float      rotation() const noexcept { return rotation_; }

            /// @brief Builds the matrix that maps world space to clip space for this camera.
            ///
            /// Used by `Renderer2D::setCamera()`. Games usually don't need it, but it can be used
            /// for custom rendering or screen-to-world conversions.
            ///
            /// @param viewportSize Size of the visible area in screen units (the window's logical
            ///                     size).
            /// @return The combined view-projection matrix, including the current shake offset.
            [[nodiscard]] math::Mat4 viewProjection(math::Vec2 viewportSize) const;

        private:
            void setTarget(std::function<math::Vec2()> target);
            [[nodiscard]] float currentShakeStrength() const noexcept;

            // Base state
            math::Vec2 position_ = { 0.0f, 0.0f };
            float      zoom_     = 1.0f;
            float      rotation_ = 0.0f;

            // Following
            std::function<math::Vec2()> target_;
            math::Vec2 lastTargetPos_ { 0.0f, 0.0f };
            math::Vec2 lookAhead_     { 0.0f, 0.0f };
            float      smoothing_     = 0.0f;
            float      facing_        = 1.0f;   // +1 right, -1 left
            bool       resync_        = false;
            bool       snapNext_      = false;

            // Shake (render-only offset; never touches position_)
            math::Vec2         shakeOffset_    { 0.0f, 0.0f };
            float              shakeStrength_  = 0.0f;
            float              shakeDuration_  = 0.0f;
            float              shakeRemaining_ = 0.0f;
            std::minstd_rand   rng_ { 0xF0E2Du };
    };
}