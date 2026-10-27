#pragma once

namespace forge::math
{
    struct Vec4
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        constexpr Vec4() = default;
        constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

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

        constexpr Vec4& operator+=(const Vec4& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }

        constexpr Vec4& operator-=(const Vec4& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }

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
