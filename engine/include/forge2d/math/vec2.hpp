#pragma once
#include <cmath>

struct Vec2
{
    float x{};
    float y{};

    Vec2 operator=(const Vec2& other) const
    {
        return {
            .x = other.x,
            .y = other.y
        };
    }

    Vec2 operator+(const Vec2& other) const
    {
        return {
            .x = x + other.x,
            .y = y + other.y
        };
    }

    Vec2 operator*(const Vec2& other) const
    {
        return {
            .x = x * other.x,
            .y = y * other.y
        };
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    /// Returns a unit-length copy, or a new zero vector when this vector has no length
    [[nodiscard]] Vec2 normalise() const
    {
        if (length() == 0)
        {
            return {
                .x = 0,
                .y = 0
            };
        }

        return {
            .x = x / length(),
            .y = y / length()
        };
    }

    /// Dot product
    [[nodiscard]] float dot(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    /// 2D cross product (returns scalar, the z-component)
    /// @return the scalar (the z-component)
    [[nodiscard]] float cross(const Vec2& other) const
    {
        return x * other.y - y * other.x;
    }
};
