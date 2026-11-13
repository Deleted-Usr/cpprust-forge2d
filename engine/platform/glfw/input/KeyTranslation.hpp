#pragma once

#include <forge2d/input/Key.hpp>

namespace forge
{
    /// @brief Converts a GLFW physical key token into the engine's independent Key enum.
    /// Unknown tokens, holes in the GLFW key range and arbitrary invalid integers return
    /// Key::Unknown. This is a pure conversion: it does not query GLFW or process events.
    /// Character generation and keyboard-layout display names are separate concerns.
    [[nodiscard]] Key translateKey(int key) noexcept;
}
