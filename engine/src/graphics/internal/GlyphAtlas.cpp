#include "GlyphAtlas.hpp"
#include <bit>
#include <opengl/OpenGLRenderer2D.hpp>

namespace forge::internal
{
    GlyphAtlas::GlyphAtlas(opengl::OpenGLRenderer2D& backend, int pageSize, std::size_t maxPagesPerFace)
    : backend_  (backend),
      pageSize_ (pageSize),
      maxPages_ (maxPagesPerFace)
    {
        if (pageSize < 4 || maxPages_ == 0)
        {
            throw std::invalid_argument("Invalid glyph atlas configuration");
        }
    }

    const AtlasGlyph& GlyphAtlas::get(const std::shared_ptr<const FontData>& face, std::uint16_t index, float size)
    {
        const auto key = (static_cast<std::uint64_t>(std::bit_cast<std::uint32_t>(size)) << 16) | index;
        auto& cache     = faces_[std::weak_ptr(face)];

        if (const auto found = cache.glyphs.find(key); found != cache.glyphs.end())
        {
            return found->second;
        }

        const auto bitmap = face->rasterise(index, size);
        if (bitmap.width == 0 || bitmap.height == 0)
        {
            return cache.glyphs.emplace(key, AtlasGlyph{}).first->second;
        }

        if (
            bitmap.width  > static_cast<unsigned>(pageSize_ - 2) ||
            bitmap.height > static_cast<unsigned>(pageSize_ - 2)
        )
        {
            throw std::length_error("Glyph does not fit an atlas page at this font size");
        }

        const int width = static_cast<int>(bitmap.width), height = static_cast<int>(bitmap.height);

        Page* page = nullptr;
        std::optional<AtlasRect> rect;

        for (auto& candidate : cache.pages)
        {
            rect = candidate.packer.insert(width, height);
            if (rect)
            {
                page = &candidate; break;
            }
        }

        if (!page)
        {
            if (cache.pages.size() == maxPages_)
            {
                throw std::length_error("Glyph atlas page limit reached for this font face");
            }

            cache.pages.push_back({backend_.createGlyphPage(pageSize_), AtlasPacker(pageSize_)});

            page = &cache.pages.back();
            rect = page->packer.insert(width, height);
        }

        backend_.uploadGlyph(
           *page->texture,
            rect->x, rect->y,
            width, height,
            {bitmap.pixels.data(), bitmap.pixels.size()}
        );

        const float scale = 1.0f / static_cast<float>(pageSize_);
































        return cache.glyphs.emplace(key, AtlasGlyph{
            page->texture,
            {rect->x * scale, rect->y * scale},
            {(rect->x + width) * scale, (rect->y + height) * scale}
        }).first->second;
    }

    void GlyphAtlas::pruneExpired()
    {
        std::erase_if(faces_, [](const auto& entry) { return entry.first.expired(); });
    }

    std::size_t GlyphAtlas::glyphCount() const
    {
        std::size_t count = 0;
        for (const auto& [face, cache] : faces_)
        {
            count += cache.glyphs.size();
        }

        return count;
    }

    std::size_t GlyphAtlas::pageCount() const
    {
        std::size_t count = 0;
        for (const auto& [face, cache] : faces_)
        {
            count += cache.pages.size();
        }

        return count;
    }
}
