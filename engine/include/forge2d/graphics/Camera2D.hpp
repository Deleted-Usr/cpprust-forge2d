#pragma once

#include <concepts>
#include <functional>
#include <random>

#include <forge2d/math/Mat4.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// Anything with a `position()` returning a world position can be followed.
    template<typename T>
    concept Followable = requires(const T& t) { { t.position() } -> std::convertible_to<math::Vec2>; };

    /// A 2D camera. position() is the world point at the CENTRE of the screen.
    /// +x right, +y down.
    ///
    /// Owned by game code: call update() once per frame in Game::update(),
    /// then pass it to Renderer2D::setCamera() in Game::render().
    class Camera2D
    {
        public:
            // ---- Configuration ------------------------------------------------
            void setPosition(math::Vec2 position); // jumps immediately; overridden next update() while following
            void setZoom(float zoom);              // > 0. 2 = everything drawn twice as big
            void setRotation(float radians);
            void setSmoothing(float seconds);      // time constant; 0 = rigidly locked to the target
            void setLookAhead(math::Vec2 offset);  // x flips with the target's horizontal facing, y is the constant

            // ---- Following ----------------------------------------------------
            /// Follows `target` BY REFERENCE: it must outlive the follow
            /// (or call `follow()`/`stopFollowing()` before it's destroyed)
            template<Followable T>
            void follow(const T& target)
            {
                setTarget([&target] { return math::Vec2(target.position()); });
            }

            /// Following a temporary would dangle immediately: make it a compile error
            template<Followable T>
            void follow(const T&&) = delete;

            /// Escape hatch for things without position(), e.g. the midpoint of two players:
            /// `cam.follow([&] { return (p1.position() + p2.position()) * 0.5f; });`
            void follow(std::function<math::Vec2()> targetPosition)
            {
                setTarget(std::move(targetPosition));
            }

            void stopFollowing();

            /// Skip smoothing for one update, e.g. after a respawn or level load.
            void snapToTarget() noexcept { snapNext_ = true; }

            // ---- Effects ------------------------------------------------------
            /// Strength in world units, duration in seconds.
            /// A weaker shake never cuts off a stronger one that's still running.
            void shake(float strength, float duration);

            // ---- Per frame ----------------------------------------------------
            void update(float deltaTime);

            // ---- Queries ------------------------------------------------------
            [[nodiscard]] math::Vec2 position() const noexcept { return position_; }
            [[nodiscard]] float      zoom()     const noexcept { return zoom_; }
            [[nodiscard]] float      rotation() const noexcept { return rotation_; }

            /// Engine-facing: the renderer supplies the viewport size
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
