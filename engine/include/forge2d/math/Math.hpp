#pragma once

/// @file Math.hpp
/// @brief Angle conversion helpers.

#include <numbers>

namespace forge::math
{
    /// @brief Converts an angle from degrees to radians.
    ///
    /// The engine measures every rotation in radians; use this to write angles in degrees.
    ///
    /// @code
    /// camera.setRotation(forge::math::radians(90.0f));
    /// @endcode
    [[nodiscard]]
    constexpr float radians(const float degrees)
    {
        return degrees * std::numbers::pi_v<float> / 180.0f;
    }

    /// @brief Converts an angle from radians to degrees.
    [[nodiscard]]
    constexpr float degrees(const float radians)
    {
        return radians * 180.0 / std::numbers::pi_v<float>;
    }
}
