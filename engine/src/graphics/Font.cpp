#include <forge2d/graphics/Font.hpp>

#include <forge2d/assets/AssetError.hpp>
#include <graphics/internal/FontData.hpp>

#include <stdexcept>

namespace forge::internal
{
    FontData::FontData(std::span<const std::uint8_t> bytes)
        try : face_(rust::create_font_face({bytes.data(), bytes.size()})) {}
        catch (const ::rust::Error& error) { throw AssetError(error.what()); }

    rust::TextLayout FontData::layout(std::string_view text, float size) const
    {
        try
        {
            return face_->layout({reinterpret_cast<const std::uint8_t*>(text.data()), text.size()}, size);
        }
        catch (const ::rust::Error& error)
        {
            throw std::invalid_argument(error.what());
        }
    }

    rust::GlyphBitmap FontData::rasterise(std::uint16_t index, float size) const
    {
        return face_->rasterise(index, size);
    }
}

namespace forge
{
    math::Vec2 Font::measureText(std::string_view text, float size) const
    {
        if (!data_ || text.empty())
        {
            return {};
        }

        const auto layout = data_->layout(text, size);

        return {layout.width, layout.height};
    }
}
