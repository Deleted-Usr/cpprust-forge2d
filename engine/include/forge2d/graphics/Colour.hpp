#pragma once

#include <forge2d/math/Vec4.hpp>

namespace forge
{
    struct Colour
    {
        float r{};
        float g{};
        float b{};
        float a = 1.0f;

        [[nodiscard]]
        constexpr math::Vec4 toVec4() const
        {
            return {
                r, g, b, a
            };
        }
    };
}
