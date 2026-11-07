#pragma once

#include <forge2d/math/Mat4.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// +y right, -y down, origin top-left of the viewport when position == (0, 0)
    class Camera2D
    {
        public:
            math::Vec2 position { 0.0f, 0.0f };
            float      zoom     = 1.0f;
            float      rotation = 0.0f; // radians

            [[nodiscard]]
            math::Mat4 viewProjection(math::Vec2 viewportSize) const
            {
                const auto projection = math::Mat4::orthographic(0.0f, viewportSize.x, viewportSize.y, 0.0f);

                // View = inverse of the camera's own transform
                const math::Mat4 view = math::Mat4::scale({ zoom, zoom })
                                      * math::Mat4::rotate(-rotation)
                                      * math::Mat4::translate({ -position.x, -position.y });

                return projection * view;
            }
    };
}
