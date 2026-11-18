#pragma once

#include <functional>
#include "BatchBuilder2D.hpp"
#include "GlyphAtlas.hpp"

namespace forge::internal
{
    /// @brief Engine-internal: turns `Renderer2D::drawText()` calls into glyph quads.
    ///
    /// Lays text out through the font's Rust face, fetches each glyph from the `GlyphAtlas`
    /// (rasterising it on a cache miss) and appends one textured quad per visible glyph to the
    /// renderer's batch, in draw order.
    class TextRenderer
    {
        public:
            explicit TextRenderer(opengl::OpenGLRenderer2D& backend) : atlas_(backend) {}

            /// @brief Frees atlas caches of fonts destroyed since the last frame. Called by
            ///        `Renderer2D::beginFrame()`, after the previous frame's batches were submitted.
            void beginFrame() { atlas_.pruneExpired(); }

            /// @brief Appends the glyphs of @p text to @p batches.
            ///
            /// @param flush Called when the batch is full (out of vertices, indices or texture
            ///              slots); the glyph is then retried in the empty batch.
            ///
            /// @pre @p font is valid.
            /// @throws std::invalid_argument If @p size is invalid (from layout).
            /// @throws std::length_error     If the font's atlas runs out of space.
            void draw(
                std::string_view text,
                const Font& font,
                math::Vec2 position,
                float size,
                Colour colour,
                BatchBuilder2D& batches,
                const std::function<void()>& flush
            );

        private:
            GlyphAtlas atlas_;
    };
}
