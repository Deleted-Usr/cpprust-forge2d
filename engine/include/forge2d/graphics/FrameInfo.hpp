#pragma once

#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// Engine-facing drawing dimensions. Logical coordinates may differ from pixels.
    struct FrameInfo
    {
        math::Vec2 logicalSize{};

        int framebufferWidth = 0;
        int framebufferHeight = 0;
    };
}
