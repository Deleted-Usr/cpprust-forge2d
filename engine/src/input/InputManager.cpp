#include <forge2d/input/InputManager.hpp>


namespace
{
    /// @brief Appends the shortest UTF-8 encoding of one Unicode scalar value.
    /// Surrogates and values beyond U+10FFFF are replaced with U+FFFD.
    /// Encoding layout: https://www.rfc-editor.org/rfc/rfc3629#section-3
    void appendUtf8(std::string& text, char32_t codepoint)
    {
        if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
        {
            codepoint = 0xFFFD;
        }

        if (codepoint <= 0x7F)
        {
            text.push_back(static_cast<char>(codepoint));
        }
        else if (codepoint <= 0x7FF)
        {
            text.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else if (codepoint <= 0xFFFF)
        {
            text.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            text.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else
        {
            text.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            text.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
    }

    /// @brief Records real transitions without erasing earlier edges in this snapshot.
    /// Duplicate states are ignored, so OS/synthetic releases cannot generate extra edges.
    /// A complete tap preserves pressed and released even though its final down state is false.
    void setDown(forge::internal::ButtonState& state, bool down)
    {
        if (state.down == down) return;

        state.down = down;

        if (down)
        {
            state.pressed = true;
        }
        else
        {
            state.released = true;
        }
    }

    /// @brief Returns button state or an immutable all-false fallback for invalid enum values.
    /// Index zero is reserved for Unknown; the upper bound excludes Count and larger values.
    template<typename Button, std::size_t N>
    const forge::internal::ButtonState& stateFor(const std::array<forge::internal::ButtonState, N>& states, Button button)
    {
        static constexpr forge::internal::ButtonState empty{};
        const auto index = static_cast<std::size_t>(button);

        return index > 0 && index < N ? states[index] : empty;
    }
}

namespace forge
{
    void InputManager::beginFrame(std::span<const internal::InputEvent> events)
    {
        for (auto& state : keys_)  state.pressed = state.released = false;
        for (auto& state : mouse_) state.pressed = state.released = false;

        mouseDelta_  = {};
        scrollDelta_ = {};

        text_.clear();

        // Arrival order matters: focus changes affect whether subsequent presses,
        // text and scroll are accepted, and taps may contain both transitions.
        for (const auto& event : events)
        {
            std::visit([this](const auto& value) { apply(value); }, event);
        }
    }

    bool InputManager::isKeyDown     (Key key) const { return stateFor(keys_, key).down; }
    bool InputManager::isKeyPressed  (Key key) const { return stateFor(keys_, key).pressed; }
    bool InputManager::isKeyReleased (Key key) const { return stateFor(keys_, key).released; }

    bool InputManager::isMouseButtonDown     (MouseButton button) const { return stateFor(mouse_, button).down; }
    bool InputManager::isMouseButtonPressed  (MouseButton button) const { return stateFor(mouse_, button).pressed; }
    bool InputManager::isMouseButtonReleased (MouseButton button) const { return stateFor(mouse_, button).released; }

    void InputManager::apply(const internal::KeyEvent& event)
    {
        const auto index = static_cast<std::size_t>(event.key);

        if (index > 0 && index < keys_.size() && (!event.down || focused_))
        {
            setDown(keys_[index], event.down);
        }
    }

    void InputManager::apply(const internal::MouseButtonEvent& event)
    {
        const auto index = static_cast<std::size_t>(event.button);
        if (index > 0 && index < mouse_.size() && (!event.down || focused_))
        {
            setDown(mouse_[index], event.down);
        }
    }

    void InputManager::apply(const internal::CursorEvent& event)
    {
        const math::Vec2 position{static_cast<float>(event.x), static_cast<float>(event.y)};

        if (focused_ && hasCursorPosition_)
        {
            mouseDelta_ += position - mousePosition_;
        }

        mousePosition_     = position;
        hasCursorPosition_ = focused_;
    }

    void InputManager::apply(const internal::ScrollEvent& event)
    {
        if (focused_) scrollDelta_ += {static_cast<float>(event.x), static_cast<float>(event.y)};
    }

    void InputManager::apply(const internal::TextEvent& event)
    {
        if (focused_) appendUtf8(text_, event.codepoint);
    }

    void InputManager::apply(const internal::FocusEvent& event)
    {
        focused_           = event.focused;
        hasCursorPosition_ = false;

        if (!focused_)
        {
            for (auto& state : keys_)  setDown(state, false);
            for (auto& state : mouse_) setDown(state, false);

            mouseDelta_ = {};
        }
    }
}
