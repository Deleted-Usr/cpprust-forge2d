#include "TestSupport.hpp"

#include <forge2d/input/InputManager.hpp>

#include <initializer_list>
#include <stdexcept>
#include <type_traits>

int main()
{
    using namespace forge;
    using namespace forge::internal;

    InputManager input;
    static_assert(std::is_same_v<decltype(input.textInput()), std::string_view>);

    const auto frame = [&](std::initializer_list<InputEvent> events)
    {
        input.beginFrame(std::span<const InputEvent>{events.begin(), events.size()});
    };

    try
    {
        frame({KeyEvent{Key::D, true}, MouseButtonEvent{MouseButton::Left, true}});

        check(input.isKeyDown(Key::D) && input.isKeyPressed(Key::D), "key press");
        check(input.isKeyPressed(Key::D), "queries must not consume presses");
        check(input.isMouseButtonDown(MouseButton::Left), "mouse press");

        frame({});

        check(input.isKeyDown(Key::D) && !input.isKeyPressed(Key::D), "held across frames");
        check(input.isMouseButtonDown(MouseButton::Left), "mouse held across frames");

        frame({KeyEvent{Key::D, true}});

        check(!input.isKeyPressed(Key::D), "duplicate press is not a new edge");

        frame({KeyEvent{Key::D, false}, MouseButtonEvent{MouseButton::Left, false}});

        check(!input.isKeyDown(Key::D) && input.isKeyReleased(Key::D), "key release");
        check(input.isMouseButtonReleased(MouseButton::Left), "mouse release");

        frame(
            {
                KeyEvent{Key::Space, true},
                KeyEvent{Key::Space, false},
                MouseButtonEvent{MouseButton::Right, true},
                MouseButtonEvent{MouseButton::Right, false}
            }
        );

        check(!input.isKeyDown(Key::Space) && input.isKeyPressed(Key::Space) && input.isKeyReleased(Key::Space), "short key tap must survive");

        check(
              !input.isMouseButtonDown(MouseButton::Right)
              && input.isMouseButtonPressed(MouseButton::Right)
              && input.isMouseButtonReleased(MouseButton::Right),
              "short mouse tap must survive"
        );

        frame({});

        check(!input.isKeyPressed(Key::Space) && !input.isKeyReleased(Key::Space), "edges reset");

        frame({CursorEvent{100, 200}});

        check(input.mouseDelta().x == 0 && input.mouseDelta().y == 0, "initial cursor baseline");

        frame(
            {
                CursorEvent{105, 202}, CursorEvent{110, 208},
                ScrollEvent{0.5, 1.5}, ScrollEvent{-0.25, -0.5},
                TextEvent{U'A'}, TextEvent{U'\U0001F600'}
            }
        );

        check(input.mousePosition().x == 110   && input.mousePosition().y == 208, "cursor position");
        check(input.mouseDelta().x    == 10    && input.mouseDelta().y    == 8,   "cursor accumulation and Y sign");
        check(input.scrollDelta().x   == 0.25f && input.scrollDelta().y   == 1,   "fractional scroll accumulation");

        check(input.textInput() == "A\xF0\x9F\x98\x80", "UTF-8 input");

        frame({});

        check(input.mouseDelta().x == 0 && input.scrollDelta().y == 0 && input.textInput().empty(), "frame transient reset");
        check(input.mousePosition().x == 110, "cursor position persists");

        // Explicit byte sequences keep expected output independent of source-file encoding.
        const struct
        {
            char32_t codepoint;
            std::string_view bytes;
        } encodings[] = {
            {0x0000, std::string_view{"\0", 1}},
            {0x007F, "\x7F"},
            {0x0080, "\xC2\x80"},
            {0x07FF, "\xDF\xBF"},
            {0x0800, "\xE0\xA0\x80"},
            {0xD7FF, "\xED\x9F\xBF"},
            {0xE000, "\xEE\x80\x80"},
            {0xFFFF, "\xEF\xBF\xBF"},
            {0x10000, "\xF0\x90\x80\x80"},
            {0x10FFFF, "\xF4\x8F\xBF\xBF"}
        };
        for (const auto& encoding : encodings)
        {
            frame({TextEvent{encoding.codepoint}});
            check(input.textInput() == encoding.bytes, "UTF-8 encoding boundary");
        }

        frame({TextEvent{U'A'}, TextEvent{U'\u00E9'},
               TextEvent{U'\u4E2D'}, TextEvent{U'\U0001F600'}});
        const std::string_view expectedText = "A\xC3\xA9\xE4\xB8\xAD\xF0\x9F\x98\x80";
        check(input.textInput() == expectedText, "mixed UTF-8 characters preserve event order");
        check(input.textInput().size() == 10, "UTF-8 view size counts bytes");
        check(input.textInput() == expectedText, "text queries do not consume text");
        std::string savedText;
        savedText.append(input.textInput());
        frame({});
        check(input.textInput().empty() && savedText == expectedText,
              "UTF-8 frame reset does not affect an owned copy");

        for (char32_t invalid : {0xD800, 0xDBFF, 0xDC00, 0xDFFF, 0x110000})
        {
            frame({TextEvent{invalid}, TextEvent{U'!'}});
            check(input.textInput() == "\xEF\xBF\xBD!", "invalid scalar is replaced without losing following text");
        }
        frame({TextEvent{static_cast<char32_t>(0xFFFFFFFFu)}});
        check(input.textInput() == "\xEF\xBF\xBD", "invalid maximum value is replaced");

        frame(
            {
                KeyEvent{Key::D, true},
                MouseButtonEvent{MouseButton::Left, true}
            }
        );
        frame(
            {
                FocusEvent{false},
                KeyEvent{Key::D, false},
                KeyEvent{Key::A, true},
                ScrollEvent{0, 1},
                TextEvent{U'x'}
            }
        );

        check(!input.isFocused() && !input.isKeyDown(Key::D)         && input.isKeyReleased(Key::D),                    "focus loss releases held keys");
        check(!input.isMouseButtonDown(MouseButton::Left)            && input.isMouseButtonReleased(MouseButton::Left), "focus loss releases mouse");
        check(!input.isKeyDown(Key::A) && input.scrollDelta().y == 0 && input.textInput().empty(),                      "ignore input while unfocused");

        frame({FocusEvent{true}, CursorEvent{500, 500}, CursorEvent{502, 503}});

        check(input.mouseDelta().x == 2 && input.mouseDelta().y == 3, "focus regain resets baseline");

        frame(
            {
                KeyEvent{Key::Unknown, true},
                KeyEvent{Key::Count, true},
                KeyEvent{static_cast<Key>(65535), true},
                MouseButtonEvent{MouseButton::Unknown, true},
                MouseButtonEvent{MouseButton::Count, true}
            }
        );

        check(!input.isKeyDown(Key::Unknown) && !input.isKeyDown(Key::Count) && !input.isKeyDown(static_cast<Key>(65535)), "invalid key safety");
        check(!input.isMouseButtonDown(MouseButton::Unknown) && !input.isMouseButtonDown(static_cast<MouseButton>(255)),   "invalid mouse safety");

        std::cout << "Input state tests passed\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
