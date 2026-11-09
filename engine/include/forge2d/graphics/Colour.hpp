#pragma once

#include <cstdint>

#include <forge2d/math/Vec4.hpp>

namespace forge
{
    struct Colour
    {
        float r{0.0f};
        float g{0.0f};
        float b{0.0f};
        float a{1.0f};

        [[nodiscard]]
        constexpr math::Vec4 toVec4() const
        {
            return { r, g, b, a };
        }

        // Optional: Construct from standard 0-255 integer values
        [[nodiscard]]
        static constexpr Colour fromRGBA8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        {
            return {
                .r = red   / 255.0f,
                .g = green / 255.0f,
                .b = blue  / 255.0f,
                .a = alpha / 255.0f
            };
        }
    };

    namespace colours
    {
        // Colour palette constants
        inline constexpr Colour Clear   { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f };
        inline constexpr Colour Black   { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };
        inline constexpr Colour White   { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
        inline constexpr Colour Red     { .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };
        inline constexpr Colour Green   { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
        inline constexpr Colour Blue    { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };
        inline constexpr Colour Magenta { .r = 1.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };
        inline constexpr Colour Yellow  { .r = 1.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
        inline constexpr Colour Cyan    { .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
    }
}
