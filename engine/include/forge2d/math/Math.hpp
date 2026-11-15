#pragma once

/// @file Math.hpp
/// @brief Angle conversion helpers.

#include <numbers>
#include <random>

namespace forge::math
{
    namespace
    {
        std::random_device rd;
        std::mt19937 gen(rd());
    }

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

    /// @brief Generates a random @p double between @p min inclusive and @p max exclusive
    ///
    /// @param min the smallest number that can be generated (inclusive)
    /// @param max the exclusive upper bound; generated values will be strictly less than this
    /// @returns a @p double between @p min inclusive and @p max exclusive
    [[nodiscard]]
    inline double randomDouble(double min, double max)
    {
        std::uniform_real_distribution distribution(min, max);
        double result = distribution(gen);

        return result;
    }

    /// @brief Generates a random @p int between @p min inclusive and @p max inclusive
    ///
    /// @param min the smallest number that can be generated (inclusive)
    /// @param max the largest number that can be generated (inclusive)
    /// @returns a @p int between @p min inclusive and @p max inclusive
    [[nodiscard]]
    inline int randomInt(int min, int max)
    {
        std::uniform_int_distribution distribution(min, max);
        int result = distribution(gen);

        return result;
    }
}
