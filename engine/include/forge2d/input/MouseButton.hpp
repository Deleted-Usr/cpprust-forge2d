#pragma once

/// @file MouseButton.hpp
/// @brief Engine identifiers for mouse buttons.

#include <cstdint>

namespace forge
{
    /// @brief A mouse button, independent of the platform's own button numbers.
    ///
    /// Pass it to `InputManager::isMouseButtonDown()` and related queries.
    enum class MouseButton : std::uint8_t
    {
        Unknown = 0, ///< A button the engine doesn't recognise. Queries always return `false`.

        Left,    ///< Primary button.
        Right,   ///< Secondary button.
        Middle,  ///< Wheel or middle button.
        Button4, ///< Extra button, often "back".
        Button5, ///< Extra button, often "forward".
        Button6, ///< Extra button.
        Button7, ///< Extra button.
        Button8, ///< Extra button.

        /// Number of values, for sizing arrays. Not a valid button.
        Count
    };
}
