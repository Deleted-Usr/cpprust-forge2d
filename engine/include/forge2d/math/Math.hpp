#pragma once

#include <numbers>

namespace forge::math
{
    [[nodiscard]]
    constexpr float radians(const float degrees)
    {
        return degrees * std::numbers::pi_v<float> / 180.0f;
    }

    [[nodiscard]]
    constexpr float degrees(const float radians)
    {
        return radians * 180.0 / std::numbers::pi_v<float>;
    }
}
