#pragma once

#include <span>
#include <forge2d/graphics/Colour.hpp>
#include <forge2d/math/Transform2D.hpp>
#include "RenderBatch2D.hpp"

namespace forge::internal
{
    /// CPU-only geometry builder. A false return leaves the batch unchanged;
    /// the caller submits, clears, then retries the primitive.
    class BatchBuilder2D
    {
        public:
            static constexpr std::size_t CircleSegments = 64;

            BatchBuilder2D(
                std::shared_ptr<const GPUTexture> whiteTexture,
                std::size_t maxVertices,
                std::size_t maxIndices,
                std::size_t maxTextures
            );

            bool tryAppendRect     (const math::Transform2D& transform, Colour colour, bool fill);
            bool tryAppendTriangle (const math::Transform2D& transform, Colour colour, bool fill);
            bool tryAppendCircle   (math::Vec2 centre, float radius,    Colour colour, bool fill);
            bool tryAppendLine     (math::Vec2 from, math::Vec2 to,     Colour colour, float thickness);

            bool tryAppendSprite   (const std::shared_ptr<const GPUTexture>& texture, const math::Transform2D& transform, Colour tint);

            [[nodiscard]] const RenderBatch2D& batch() const { return batch_; }
            void clear() { batch_.clear(); }

        private:
            bool appendPolygon(std::span<const math::Vec2> points, Colour colour, bool fill);
            bool append(
                std::span<const Vertex2D>                vertices,
                std::span<const std::uint32_t>           indices,
                const std::shared_ptr<const GPUTexture>& texture
            );

            RenderBatch2D batch_;
            std::shared_ptr<const GPUTexture> whiteTexture_;
            std::size_t maxVertices_, maxIndices_, maxTextures_;
    };
}
