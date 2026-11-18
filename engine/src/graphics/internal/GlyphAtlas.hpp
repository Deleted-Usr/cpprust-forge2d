#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include "AtlasPacker.hpp"
#include "FontData.hpp"

namespace forge::internal
{
    class GPUTexture;
    namespace opengl { class OpenGLRenderer2D; }

    /// @brief Where one cached glyph lives: its atlas page and UV rectangle. An empty
    ///        `texture` means the glyph has no visible pixels.
    struct AtlasGlyph
    {
        std::shared_ptr<const GPUTexture> texture;
        math::Vec2 uvMin, uvMax;
    };

    /// @brief Engine-internal cache of rasterised glyphs in R8 atlas pages.
    ///
    /// Glyphs are keyed by face, exact float size and glyph index. Each live face has its own
    /// append-only pages (default 16 pages of 1024×1024), shared by all its sizes. Glyphs never
    /// move once packed, so returned UVs stay valid while the face's cache exists.
    class GlyphAtlas
    {
        public:
            explicit GlyphAtlas(opengl::OpenGLRenderer2D& backend, int pageSize = 1024, std::size_t maxPagesPerFace = 16);
            /// @brief Returns the cached glyph, rasterising and uploading it on a miss.
            ///
            /// @throws std::length_error If the glyph is larger than a page, or the face has used
            ///         all its pages.
            const AtlasGlyph& get(const std::shared_ptr<const FontData>& face, std::uint16_t index, float size);

            /// @brief Drops the caches (and page references) of faces whose `Font` handles have all
            ///        been destroyed.
            void pruneExpired();

            [[nodiscard]] std::size_t glyphCount() const;
            [[nodiscard]] std::size_t pageCount()  const;

        private:
            struct Page
            {
                std::shared_ptr<const GPUTexture> texture;
                AtlasPacker packer;
            };

            struct FaceCache
            {
                std::unordered_map<std::uint64_t, AtlasGlyph> glyphs;
                std::vector<Page> pages;
            };

            opengl::OpenGLRenderer2D& backend_;

            int pageSize_;
            std::size_t maxPages_;
        
            std::map<
                std::weak_ptr<const FontData>, FaceCache,
                std::owner_less<std::weak_ptr<const FontData>>
            > faces_;
    };
}
