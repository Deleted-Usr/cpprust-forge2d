#pragma once

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Mat4.hpp>

namespace forge::internal
{
    struct RenderView2D
    {
        math::Mat4 viewProjection;

        static RenderView2D screen(math::Vec2 logicalSize)
        {
            return {
                math::Mat4::orthographic(
                    0.0f, logicalSize.x,
                    logicalSize.y, 0.0f
                )
            };
        }
    };
}
