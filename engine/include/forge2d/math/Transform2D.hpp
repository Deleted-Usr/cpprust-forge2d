#pragma once

/// @file Transform2D.hpp
/// @brief Position, rotation and scale of a 2D object.

#include <forge2d/math/Mat4.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge::math
{
    /// @brief Where a 2D object is, how it's turned, and how big it is.
    ///
    /// `Renderer2D`'s draw functions take one to place each shape or sprite. There, `position` is
    /// the shape's **top-left corner**, `scale` is its width and height, and `rotation` turns it
    /// around that corner. Designated initialisers keep calls readable, and any field left out
    /// keeps its default:
    ///
    /// @code
    /// renderer.drawRect({ .position = { 16, 16 }, .scale = { 200, 20 } }, forge::colours::Red);
    /// renderer.drawSprite(tex, { .position = pos, .rotation = forge::math::radians(45.0f), .scale = { 32, 32 } });
    /// @endcode
    struct Transform2D
    {
        /// Translation. Defaults to `{ 0, 0 }`.
        Vec2 position {
            0.0f,
            0.0f
        };

        /// Rotation in radians around `position`. A positive angle turns clockwise on screen,
        /// because +Y points down. Defaults to `0`.
        float rotation = 0.0f;

        /// Scale on each axis. The renderer treats it as the drawn width and height. Defaults to
        /// `{ 1, 1 }`.
        Vec2 scale {
            1.0f,
            1.0f
        };

        /// @brief Builds the matrix for this transform.
        ///
        /// @return `translate(position) * rotate(rotation) * scale(scale)`: points are scaled,
        ///         then rotated around the origin, then moved by `position`.
        [[nodiscard]]
        Mat4 matrix() const;
    };
}
