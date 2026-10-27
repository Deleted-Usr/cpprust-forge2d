#pragma once

#include <cmath>

namespace forge::math
{
    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;

        constexpr Vec2() = default;
        constexpr Vec2(float x, float y) : x(x), y(y) {}

        [[nodiscard]]
        constexpr Vec2 operator+(const Vec2& other) const
        {
            return {
                x + other.x,
                y + other.y
            };
        }

        [[nodiscard]]
        constexpr Vec2 operator-(const Vec2& other) const
        {
            return {
                x - other.x,
                y - other.y
            };
        }

        [[nodiscard]]
        constexpr Vec2 operator*(float scalar) const
        {
            return {
                x * scalar,
                y * scalar
            };
        }

        [[nodiscard]]
        constexpr Vec2 operator/(float scalar) const
        {
            return {
                x / scalar,
                y / scalar
            };
        }

        constexpr Vec2& operator+=(const Vec2& other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }

        constexpr Vec2& operator-=(const Vec2& other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }

        [[nodiscard]]
        float length() const
        {
            return std::sqrt(x * x + y * y);
        }

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

        [[nodiscard]]
        constexpr float dot(const Vec2& other) const
        {
            return x * other.x + y * other.y;
        }
    };
}
