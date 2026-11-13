#pragma once

#include <forge2d/input/MouseButton.hpp>

namespace forge
{
    /// @brief Converts any of GLFW's eight button tokens into an engine mouse button.
    /// GLFW's left/right/middle aliases map to Left/Right/Middle; buttons 4-8 retain
    /// their numbered identities. Unsupported integers return MouseButton::Unknown.
    /// This is a pure conversion and does not query current button state.
    [[nodiscard]] MouseButton translateMouseButton(int button) noexcept;
}
