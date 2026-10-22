#pragma once

struct Vec2
{
    float x{};
    float y{};

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
};
