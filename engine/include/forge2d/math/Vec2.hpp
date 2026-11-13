#pragma once

/// @file Vec2.hpp
/// @brief 2D vectors for positions, sizes and directions.

#include <cmath>

namespace forge::math
{
    /// @brief A 2D vector of floats, used for positions, sizes, offsets and directions.
    ///
    /// In screen and world space, +X points right and +Y points **down**. A default-constructed
    /// vector is `{ 0, 0 }`.
    ///
    /// @code
    /// forge::math::Vec2 position { 100.0f, 50.0f };
    /// forge::math::Vec2 velocity = (target - position).normalized() * speed;
    /// position += velocity * deltaTime;
    /// @endcode
    struct Vec2
    {
        float x = 0.0f; ///< Horizontal component. Positive is right.
        float y = 0.0f; ///< Vertical component. Positive is down.

        /// @brief Creates the zero vector.
        constexpr Vec2() = default;

        /// @brief Creates a vector from its components.
        constexpr Vec2(float x, float y) : x(x), y(y) {}

        /// @brief Component-wise sum.
        [[nodiscard]]
        constexpr Vec2 operator+(const Vec2& other) const
        {
            return {
                x + other.x,
                y + other.y
            };
        }

        /// @brief Component-wise difference.
        [[nodiscard]]
        constexpr Vec2 operator-(const Vec2& other) const
        {
            return {
                x - other.x,
                y - other.y
            };
        }

        /// @brief Multiplies both components by @p scalar.
        ///
        /// Only `vector * scalar` is supported, not `scalar * vector`.
        [[nodiscard]]
        constexpr Vec2 operator*(float scalar) const
        {
            return {
                x * scalar,
                y * scalar
            };
        }

        /// @brief Divides both components by @p scalar.
        ///
        /// Dividing by zero follows IEEE float rules (giving infinities or NaN); it isn't checked.
        [[nodiscard]]
        constexpr Vec2 operator/(float scalar) const
        {
            return {
                x / scalar,
                y / scalar
            };
        }

        /// @brief Adds @p other to this vector.
        constexpr Vec2& operator+=(const Vec2& other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }

        /// @brief Subtracts @p other from this vector.
        constexpr Vec2& operator-=(const Vec2& other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }

        /// @brief The vector's length (Euclidean magnitude).
        [[nodiscard]]
        float length() const
        {
            return std::sqrt(x * x + y * y);
        }

        /// @brief Returns a vector pointing the same way with a length of 1.
        ///
        /// @return The unit vector, or `{ 0, 0 }` for the zero vector (rather than NaN).
        [[nodiscard]]
        Vec2 normalized() const
        {
            const float len = length();

            if (len == 0.0f)
            {
                return {};
            }

            return {
                x / len,
                y / len
            };
        }

        /// @brief The dot product with @p other.
        ///
        /// For unit vectors it is the cosine of the angle between them: `1` for the same
        /// direction, `0` at right angles and `-1` for opposite directions.
        [[nodiscard]]
        constexpr float dot(const Vec2& other) const
        {
            return x * other.x + y * other.y;
        }
    };
}
