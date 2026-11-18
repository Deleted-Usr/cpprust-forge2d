#include "TextRenderer.hpp"

#include <forge2d/debug/Assert.hpp>

namespace forge::internal
{
    void TextRenderer::draw(
        std::string_view             text,
        const Font&                  font,
        math::Vec2                   position,
        float                        size,
        Colour                       colour,
        BatchBuilder2D&              batches,
        const std::function<void()>& flush
    )
    {
        const std::shared_ptr<const FontData>& face = FontAccess::data(font);
        const auto layout = face->layout(text, size);

        for (const auto& placement : layout.glyphs)
        {
            const auto& glyph = atlas_.get(face, placement.index, size);

            if (!glyph.texture) continue;

            const auto append = [&] {
                return batches.tryAppendGlyph(
                    glyph.texture,
                    position + math::Vec2{placement.x, placement.y},
                    { static_cast<float>(placement.width), static_cast<float>(placement.height) },
                    glyph.uvMin,
                    glyph.uvMax,
                    colour
                );
            };

            if (!append())
            {
                flush();
                FORGE_CHECK(append(), "Glyph cannot fit in an empty batch");
            }
        }
    }
}
