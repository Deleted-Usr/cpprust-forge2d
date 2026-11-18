#pragma once

#include <span>

#include <rust/cxx.h>
#include <forge2d_rust/fonts.h>

#include <forge2d/graphics/Font.hpp>

namespace forge::internal
{
    /// Owns the opaque Rust face through CXX's allocator-safe Box.
    class FontData
    {
        public:
            explicit FontData(std::span<const std::uint8_t> bytes);

            [[nodiscard]] rust::TextLayout layout(std::string_view text, float size)  const;
            [[nodiscard]] rust::GlyphBitmap rasterise(std::uint16_t index, float size) const;

        private:
            ::rust::Box<rust::FontFace> face_;
    };

    struct FontAccess
    {
        static Font create(std::span<const std::uint8_t> bytes)
        {
            Font font;
            font.data_ = std::make_shared<const FontData>(bytes);

            return font;
        }

        static const std::shared_ptr<const FontData>& data(const Font& font) { return font.data_; }
    };
}
