#include <forge2d/graphics/Camera2D.hpp>

#include <algorithm>
#include <cmath>

#include <forge2d/debug/Assert.hpp>

namespace forge
{
    namespace
    {
        // Below this horizontal speed (world units/s) the camera keeps its old facing,
        // so standing still or tiny jitter doesn't flip the look-ahead back and forth.
        constexpr float FacingThreshold = 1.0f;
    }

    void Camera2D::setPosition(const math::Vec2 position) { position_ = position; }
    void Camera2D::setRotation(const float radians)       { rotation_ = radians; }
    void Camera2D::setLookAhead(const math::Vec2 offset)  { lookAhead_ = offset; }

    void Camera2D::setZoom(const float zoom)
    {
        if (!FORGE_ENSURE(std::isfinite(zoom) && zoom > 0.0f, "Camera2D::setZoom: zoom must be finite and positive")) return;
        zoom_ = zoom;
    }

    void Camera2D::setSmoothing(const float seconds)
    {
        if (std::isfinite(seconds)) smoothing_ = std::max(0.0f, seconds);
    }

    void Camera2D::setTarget(std::function<math::Vec2()> target)
    {
        target_ = std::move(target);
        resync_ = true;  // don't treat the jump between targets as "velocity"
    }

    void Camera2D::stopFollowing()
    {
        target_ = nullptr;
    }

    float Camera2D::currentShakeStrength() const noexcept
    {
        if (shakeRemaining_ <= 0.0f) return 0.0f;

        const float t = shakeRemaining_ / shakeDuration_;  // 1 → 0
        return shakeStrength_ * t * t;                     // quadratic falloff feels less abrupt
    }

    void Camera2D::shake(const float strength, const float duration)
    {
        if (!std::isfinite(duration) || !std::isfinite(strength) ||
            duration <= 0.0f || strength < 0.0f || strength < currentShakeStrength()) return;

        shakeStrength_  = strength;
        shakeDuration_  = duration;
        shakeRemaining_ = duration;
    }

    void Camera2D::update(const float deltaTime)
    {
        if (!std::isfinite(deltaTime) || deltaTime < 0.0f) return;
        if (target_)
        {
            const math::Vec2 targetPosition = target_();

            if (!resync_ && deltaTime > 0.0f)
            {
                const float velocityX = (targetPosition.x - lastTargetPos_.x) / deltaTime;

                if (std::abs(velocityX) > FacingThreshold)
                {
                    facing_ = velocityX > 0.0f ? 1.0f : -1.0f;
                }
            }

            lastTargetPos_ = targetPosition;
            resync_ = false;

            const math::Vec2 goal = targetPosition + math::Vec2 { lookAhead_.x * facing_, lookAhead_.y };

            if (snapNext_ || smoothing_ <= 0.0f)
            {
                position_ = goal;
                snapNext_ = false;
            }
            else
            {
                // Frame-rate-independent exponential smoothing. `smoothing_` is the time
                // constant: after that many seconds the camera has closed ~63% of the gap.
                // (A plain lerp(pos, goal, 0.1f) would move faster at 144 Hz than at 60 Hz.)
                const float t = 1.0f - std::exp(-deltaTime / smoothing_);
                position_ = position_ + (goal - position_) * t;
            }
        }

        if (shakeRemaining_ > 0.0f)
        {
            shakeRemaining_ = std::max(0.0f, shakeRemaining_ - deltaTime);

            std::uniform_real_distribution unit(-1.0f, 1.0f);
            shakeOffset_ = math::Vec2 { unit(rng_), unit(rng_) } * currentShakeStrength();
        }
        else
        {
            shakeOffset_ = { 0.0f, 0.0f };
        }
    }

    math::Mat4 Camera2D::viewProjection(const math::Vec2 viewportSize) const
    {
        const math::Mat4 projection = math::Mat4::orthographic(0.0f, viewportSize.x, viewportSize.y, 0.0f);
        const math::Vec2 eye        = position_ + shakeOffset_;

        // Read right-to-left: move the world so `eye` is at the origin, zoom and rotate
        // around it, then move the origin to the centre of the screen.
        const math::Mat4 view = math::Mat4::translate(viewportSize * 0.5f)
                              * math::Mat4::rotate(-rotation_)
                              * math::Mat4::scale({ zoom_, zoom_ })
                              * math::Mat4::translate({ -eye.x, -eye.y });

        return projection * view;
    }
}
