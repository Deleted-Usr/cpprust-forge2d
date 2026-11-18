#pragma once

/// @file Font.hpp
/// @brief Copyable handles to loaded font faces, for drawing and measuring text.

#include <memory>
#include <string_view>
#include <forge2d/math/Vec2.hpp>

namespace forge
{
    namespace internal { class FontData; struct FontAccess; }

    /// @brief A lightweight, copyable handle to a loaded font face (a TrueType or OpenType file).
    ///
    /// Copying a `Font` does **not** copy the font: every copy refers to the same face, which is
    /// reference counted and freed when the last handle is destroyed. A copy keeps its face alive
    /// on its own.
    ///
    /// A face isn't tied to one size. Pass the size to each `Renderer2D::drawText()` or
    /// `measureText()` call; the same handle can draw a 16-unit label and a 64-unit title.
    ///
    /// A default-constructed `Font` is empty (`valid() == false`) until something is assigned to
    /// it. Drawing with an empty font draws nothing, and measuring with one returns `{ 0, 0 }`.
    ///
    /// Get fonts from `AssetManager::loadFont()`.
    ///
    /// @code
    /// forge::Font font = assets.loadFont("fonts/MyFont.ttf");
    ///
    /// // Centre a title in the window.
    /// const forge::math::Vec2 titleSize = font.measureText("Game Over", 48);
    /// renderer.drawText("Game Over", font, forge::Screen::centre() - titleSize / 2.0f, 48);
    /// @endcode
    ///
    /// @note A `Font` only holds CPU memory. Glyph textures are created and cached by the
    ///       renderer the first time each glyph is drawn at each size, so creating, copying and
    ///       measuring fonts doesn't need an OpenGL context.
    class Font
    {
        public:
            /// @brief Creates an empty handle (`valid() == false`).
            Font() = default;

            /// @brief Whether this handle refers to a loaded font face.
            [[nodiscard]] bool valid() const noexcept { return data_ != nullptr; }

            /// @brief Same as `valid()`, so a handle can be used in an `if`.
            explicit operator bool() const noexcept { return valid(); }

            /// @brief True if both handles refer to the same font face. Two empty handles are equal.
            friend bool operator==(const Font&, const Font&) = default;

            /// @brief Measures the box @p text takes up when drawn at @p size.
            ///
            /// Uses exactly the same layout as `Renderer2D::drawText()`, so the result lines up
            /// with drawn text. It measures the typographic line boxes, not the visible ink:
            /// - the width is the widest line's advance, including spaces, tabs and kerning;
            /// - the height is the number of lines times the font's line height, so text with a
            ///   trailing newline is one line taller. Whitespace-only text is still one line tall.
            ///
            /// Some glyphs (such as italics or a descender on the last line) can draw slightly
            /// outside this box.
            ///
            /// @param text UTF-8 text. `\n`, `\r` and `\r\n` start new lines. Malformed UTF-8 is
            ///             measured as U+FFFD (the replacement character).
            /// @param size Font size in logical units per em, greater than `0` and at most `512`.
            ///             This is the font's design size, not the height of any letter: capitals
            ///             are usually about 0.7× it.
            /// @return Width and height in logical units, or `{ 0, 0 }` if @p text is empty or the
            ///         handle is empty.
            ///
            /// @throws std::invalid_argument If @p text isn't empty and @p size is zero, negative,
            ///         non-finite or greater than `512`, or if the font has no usable horizontal
            ///         line metrics.
            ///
            /// @note Lays the text out on every call. Cache the result if the text doesn't change.
            [[nodiscard]] math::Vec2 measureText(std::string_view text, float size) const;

        private:
            friend struct internal::FontAccess;
            std::shared_ptr<const internal::FontData> data_;
    };
}
