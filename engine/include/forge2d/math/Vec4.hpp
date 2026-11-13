#pragma once

/// @file Vec4.hpp
/// @brief 4D vectors, used for colours and homogeneous coordinates.

namespace forge::math
{
    /// @brief A 4D vector of floats.
    ///
    /// Mainly used for homogeneous coordinates with `Mat4` (a 2D point is `{ x, y, 0, 1 }`) and
    /// for colours sent to shaders (see `Colour::toVec4()`). A default-constructed vector is all
    /// zeros.
    ///
    /// @code
    /// // Transform a 2D point with a matrix.
    /// forge::math::Vec4 p = matrix * forge::math::Vec4{ point.x, point.y, 0.0f, 1.0f };
    /// @endcode
    struct Vec4
    {
        float x = 0.0f; ///< First component.
        float y = 0.0f; ///< Second component.
        float z = 0.0f; ///< Third component.
        float w = 0.0f; ///< Fourth component. `1` for points and `0` for directions.

        /// @brief Creates the zero vector.
        constexpr Vec4() = default;

        /// @brief Creates a vector from its components.
        constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        /// @brief Component-wise sum.
        [[nodiscard]]
        constexpr Vec4 operator+(const Vec4& other) const
        {
            return {
                x + other.x,
                y + other.y,
                z + other.z,
                w + other.w
            };
        }

        /// @brief Component-wise difference.
        [[nodiscard]]
        constexpr Vec4 operator-(const Vec4& other) const
        {
            return {
                x - other.x,
                y - other.y,
                z - other.z,
                w - other.w
            };
        }

        /// @brief Multiplies every component, including `w`, by @p scalar.
        [[nodiscard]]
        constexpr Vec4 operator*(float scalar) const
        {
            return {
                x * scalar,
                y * scalar,
                z * scalar,
                w * scalar
            };
        }

        /// @brief Divides every component, including `w`, by @p scalar. Division by zero isn't
        ///        checked.
        [[nodiscard]]
        constexpr Vec4 operator/(float scalar) const
        {
            return {
                x / scalar,
                y / scalar,
                z / scalar,
                w / scalar
            };
        }

        /// @brief Adds @p other to this vector.
        constexpr Vec4& operator+=(const Vec4& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }

        /// @brief Subtracts @p other from this vector.
        constexpr Vec4& operator-=(const Vec4& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }

        /// @brief The dot product of all four components with @p other.
        [[nodiscard]]
        constexpr float dot(const Vec4& other) const
        {
            return x * other.x
                 + y * other.y
                 + z * other.z
                 + w * other.w;
        }
    };
}
