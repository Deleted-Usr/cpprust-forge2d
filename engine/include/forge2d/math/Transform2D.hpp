#pragma once

#include <forge2d/math/Mat4.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge::math
{
    struct Transform2D
    {
        Vec2 position {
            0.0f,
            0.0f
        };

        float rotation = 0.0f;

        Vec2 scale {
            1.0f,
            1.0f
        };

        [[nodiscard]]
        Mat4 matrix() const;
    };
}
