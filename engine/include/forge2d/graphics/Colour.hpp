#pragma once

/// @file Colour.hpp
/// @brief RGBA colours and a palette of named constants.

#include <cstdint>

#include <forge2d/math/Vec4.hpp>

namespace forge
{
    /// @brief An RGBA colour with floating-point channels from `0.0` to `1.0`.
    ///
    /// Alpha is straight (not premultiplied): `a = 1 is opaque and `a = 0` is fully
    /// transparent. A default-constructed colour is opaque black.
    ///
    /// @code
    /// forge::Colour sky    { .r = 0.4f, .g = 0.7f, .b = 1.0f };     // a defaults to 1
    /// forge::Colour orange = forge::Colour::fromRGBA8(255, 165, 0);
    /// forge::Colour ghost  { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 0.5f };
    /// @endcode
    struct Colour
    {
        float r{0.0f}; ///< Red, 0 to 1.
        float g{0.0f}; ///< Green, 0 to 1.
        float b{0.0f}; ///< Blue, 0 to 1.
        float a{1.0f}; ///< Alpha (opacity), 0 to 1. Defaults to opaque.

        /// @brief Returns the colour as a `Vec4` of `{ r, g, b, a }`, e.g. for shader uniforms.
        [[nodiscard]]
        constexpr math::Vec4 toVec4() const
        {
            return { r, g, b, a };
        }

        /// @brief Builds a colour from 0–255 channel values, as used by most image editors.
        ///
        /// @param red   Red, 0 to 255.
        /// @param green Green, 0 to 255.
        /// @param blue  Blue, 0 to 255.
        /// @param alpha Alpha, 0 to 255. Defaults to 255 (opaque).
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

    /// @brief Named colour constants.
    namespace colours
    {
        inline constexpr Colour Clear   { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f }; ///< Fully transparent.
        inline constexpr Colour Black   { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }; ///< Opaque black.
        inline constexpr Colour White   { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f }; ///< Opaque white; the "no tint" colour for sprites.
        inline constexpr Colour Red     { .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }; ///< Opaque red.
        inline constexpr Colour Green   { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f }; ///< Opaque green.
        inline constexpr Colour Blue    { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f }; ///< Opaque blue.
        inline constexpr Colour Magenta { .r = 1.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f }; ///< Opaque magenta.
        inline constexpr Colour Yellow  { .r = 1.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f }; ///< Opaque yellow.
        inline constexpr Colour Cyan    { .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f }; ///< Opaque cyan.
    }
}