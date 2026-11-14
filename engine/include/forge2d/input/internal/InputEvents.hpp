#pragma once

/// @file InputEvents.hpp
/// @brief Engine-internal input events passed from the platform layer to `InputManager`.

#include <variant>
#include <forge2d/input/Key.hpp>
#include <forge2d/input/MouseButton.hpp>

namespace forge::internal
{
    /// @brief A key went down or up. Operating-system repeats aren't sent.
    struct KeyEvent         { Key key; bool down; };

    /// @brief A mouse button went down or up.
    struct MouseButtonEvent { MouseButton button; bool down; };

    /// @brief The cursor moved to an absolute position, in logical window units with +Y down.
    struct CursorEvent      { double x; double y; };

    /// @brief The wheel or touchpad scrolled by a relative, possibly fractional, amount.
    ///        Positive Y is scrolling up.
    struct ScrollEvent      { double x; double y; };

    /// @brief One Unicode code point of typed text.
    /// InputManager encodes it into UTF-8 when building the public textInput() view.
    struct TextEvent        { char32_t codepoint; };

    /// @brief The window gained or lost focus.
    struct FocusEvent       { bool focused; };

    /// @brief Any input event, as queued by the platform layer and applied by
    ///        `InputManager::beginFrame()`.
    ///
    /// Engine-internal. Events don't hold any native window pointers.
    using InputEvent = std::variant<
        KeyEvent,
        MouseButtonEvent,
        CursorEvent,
        ScrollEvent,
        TextEvent,
        FocusEvent
    >;
}
