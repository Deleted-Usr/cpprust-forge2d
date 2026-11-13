#pragma once

/// @file InputManager.hpp
/// @brief A per-frame snapshot of keyboard, mouse, text and focus input.

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <forge2d/input/Key.hpp>
#include <forge2d/input/MouseButton.hpp>
#include <forge2d/input/detail/ButtonState.hpp>
#include <forge2d/input/detail/InputEvents.hpp>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// @brief The state of the keyboard and mouse for the current frame.
    ///
    /// `Application` owns the input manager, advances it once per frame, and passes it to
    /// `Game::update()` by const reference. Games only call its query functions.
    ///
    /// ### Down, pressed and released
    /// - **Down** means held at the end of this frame's events. It persists across frames.
    /// - **Pressed** means it went down at least once during this frame.
    /// - **Released** means it went up at least once during this frame.
    ///
    /// A quick tap between two frames reports both pressed and released, while down is `false`.
    /// Queries never consume anything: asking twice gives the same answer. Operating-system key
    /// repeat doesn't produce extra presses (use `textInput()` for typing).
    ///
    /// ### Focus
    /// While the window isn't focused, presses, scrolling and text are ignored and
    /// `mouseDelta()` stays zero (`mousePosition()` still updates). Losing focus releases every
    /// held key and button, so nothing gets stuck down.
    ///
    /// ### Threading
    /// Not thread-safe. Use it from the main thread during `Game::update()`.
    ///
    /// @code
    /// void update(float deltaTime, const forge::InputManager& input) override
    /// {
    ///     if (input.isKeyDown(forge::Key::D))        player_.moveRight(deltaTime);
    ///     if (input.isKeyPressed(forge::Key::Space)) player_.jump();
    ///
    ///     if (input.isMouseButtonPressed(forge::MouseButton::Left))
    ///     {
    ///         shootTowards(input.mousePosition());
    ///     }
    /// }
    /// @endcode
    class InputManager
    {
        public:
            /// @brief Starts a new input frame and applies @p events to it, in order.
            ///
            /// Engine-facing: `Window::updateInput()` calls it once per frame. Games shouldn't
            /// call it.
            ///
            /// Clears last frame's pressed and released flags, mouse and scroll deltas and text,
            /// then applies each event. Held state and the mouse position carry over.
            ///
            /// @param events Events received since the previous call, oldest first.
            void endFrame(std::span<const internal::InputEvent> events);

            /// @brief Whether @p key is held down. `Key::Unknown` and invalid values return `false`.
            [[nodiscard]] bool isKeyDown     (Key key) const;

            /// @brief Whether @p key went down during this frame. `Key::Unknown` and invalid
            ///        values return `false`.
            [[nodiscard]] bool isKeyPressed  (Key key) const;

            /// @brief Whether @p key went up during this frame, including releases caused by
            ///        losing focus. `Key::Unknown` and invalid values return `false`.
            [[nodiscard]] bool isKeyReleased (Key key) const;

            /// @brief Whether @p button is held down. `MouseButton::Unknown` and invalid values
            ///        return `false`.
            [[nodiscard]] bool isMouseButtonDown     (MouseButton button) const;

            /// @brief Whether @p button went down during this frame. `MouseButton::Unknown` and
            ///        invalid values return `false`.
            [[nodiscard]] bool isMouseButtonPressed  (MouseButton button) const;

            /// @brief Whether @p button went up during this frame, including releases caused by
            ///        losing focus. `MouseButton::Unknown` and invalid values return `false`.
            [[nodiscard]] bool isMouseButtonReleased (MouseButton button) const;

            /// @brief The cursor position relative to the window's top-left corner, in logical
            ///        (screen-space) units, with +Y down.
            ///
            /// This is screen space, the same space the renderer uses when no camera is set. It
            /// isn't converted through any `Camera2D`. The position persists across frames and
            /// can be outside the window.
            [[nodiscard]] math::Vec2 mousePosition() const { return mousePosition_; }

            /// @brief How far the cursor moved during this frame, in logical units, with +Y down.
            ///
            /// Zero while unfocused. The first movement after the window gains focus only sets a
            /// starting point, so regaining focus never causes a jump.
            [[nodiscard]] math::Vec2 mouseDelta()    const { return mouseDelta_; }

            /// @brief How far the mouse wheel or touchpad scrolled during this frame.
            ///
            /// Values can be fractional on touchpads. Positive Y is scrolling up (away from the
            /// user) and positive X is scrolling right.
            [[nodiscard]] math::Vec2 scrollDelta()   const { return scrollDelta_; }

            /// @brief The text typed during this frame, as Unicode code points.
            ///
            /// Unlike key queries, this respects the keyboard layout and includes key repeat, so
            /// use it for text fields.
            ///
            /// @return A view of this frame's text. Valid until the next frame begins; copy it to
            ///         keep it longer.
            [[nodiscard]] std::u32string_view textInput() const { return text_; }

            /// @brief Whether the window has keyboard focus.
            [[nodiscard]] bool isFocused() const { return focused_; }

        private:
            void apply(const internal::KeyEvent&         event);
            void apply(const internal::MouseButtonEvent& event);
            void apply(const internal::CursorEvent&      event);
            void apply(const internal::ScrollEvent&      event);
            void apply(const internal::TextEvent&        event);
            void apply(const internal::FocusEvent&       event);

            std::array<internal::ButtonState, static_cast<std::size_t>(Key::Count)>         keys_{};
            std::array<internal::ButtonState, static_cast<std::size_t>(MouseButton::Count)> mouse_{};

            math::Vec2 mousePosition_ {};
            math::Vec2 mouseDelta_    {};
            math::Vec2 scrollDelta_   {};

            std::u32string text_;

            bool focused_           = true;
            bool hasCursorPosition_ = false;
    };
}
