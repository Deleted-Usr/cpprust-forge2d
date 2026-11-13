#pragma once

/// @file Key.hpp
/// @brief Engine identifiers for physical keyboard keys.

#include <cstdint>

namespace forge
{
    /// @brief A physical key on the keyboard, independent of the platform's own key codes.
    ///
    /// Keys are identified by **position**, named after the US layout. `Key::W` is the key to
    /// the right of Tab whatever it's labelled, so WASD controls stay in the same place on AZERTY
    /// and other layouts. For typed text, use `InputManager::textInput()` instead.
    ///
    /// Pass it to `InputManager::isKeyDown()` and related queries.
    enum class Key : std::uint16_t
    {
        Unknown = 0, ///< A key the engine doesn't recognise. Queries always return `false`.

        // Letters
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        // Number row
        Num0,         ///< 0 on the number row. The keypad keys are `Keypad0` to `Keypad9`.
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,

        // Whitespace and punctuation
        Space,
        Apostrophe,
        Comma,
        Minus,
        Period,
        Slash,
        Semicolon,
        Equal,
        LeftBracket,
        Backslash,
        RightBracket,
        GraveAccent,  ///< The grave accent (backtick) / tilde key, left of 1.
        World1,       ///< Extra key on some non-US keyboards.
        World2,       ///< Extra key on some non-US keyboards.

        // Navigation and editing
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,

        // Locks and system keys
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,

        // Function keys
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,

        // Numeric keypad
        Keypad0,
        Keypad1,
        Keypad2,
        Keypad3,
        Keypad4,
        Keypad5,
        Keypad6,
        Keypad7,
        Keypad8,
        Keypad9,
        KeypadDecimal,
        KeypadDivide,
        KeypadMultiply,
        KeypadSubtract,
        KeypadAdd,
        KeypadEnter,
        KeypadEqual,

        // Modifiers
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper,    ///< Left Windows, Command or Super key.
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,   ///< Right Windows, Command or Super key.
        Menu,         ///< The context-menu key.

        /// Number of values, for sizing arrays. Not a valid key.
        Count
    };
}
