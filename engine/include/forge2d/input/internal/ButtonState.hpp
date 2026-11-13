#pragma once

/// @file ButtonState.hpp
/// @brief Engine-internal per-button state stored by `InputManager`.

namespace forge::internal
{
    /// @brief The held state of one key or button, plus the changes seen during this frame.
    ///
    /// Engine-internal. `pressed` and `released` can both be `true` after a quick tap, and
    /// neither counts how many times the button changed. Queries leave the flags alone;
    /// `InputManager::endFrame()` clears `pressed` and `released` at the start of each frame.
    struct ButtonState
    {
        bool down     = false; ///< Held at the end of this frame. Persists across frames.
        bool pressed  = false; ///< Went from up to down at least once this frame.
        bool released = false; ///< Went from down to up at least once this frame.
    };
}
