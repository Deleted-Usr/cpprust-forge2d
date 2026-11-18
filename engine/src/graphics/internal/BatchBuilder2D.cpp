#include "BatchBuilder2D.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace forge::internal
{
    namespace
    {
        math::Vec2 transformPoint(const math::Mat4& matrix, math::Vec2 point)
        {
            const auto result = matrix * math::Vec4{point.x, point.y, 0.0f, 1.0f};
            return {result.x, result.y};
        }

        constexpr std::array<math::Vec2, 4>    Quad        = {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
        constexpr std::array<std::uint32_t, 6> QuadIndices = {0, 1, 2, 2, 3, 0};

        void lineVertices(
            math::Vec2 from,
            math::Vec2 to,
            float      thickness,
            math::Vec4 colour,
            Vertex2D*  vertices
        )
        {
            const auto direction = (to - from).normalized();
            const math::Vec2 normal{-direction.y * thickness * 0.5f, direction.x * thickness * 0.5f};

            vertices[0] = {from + normal, {}, colour, 0};
            vertices[1] = {to   + normal, {}, colour, 0};
            vertices[2] = {to   - normal, {}, colour, 0};
            vertices[3] = {from - normal, {}, colour, 0};
        }
    }

    BatchBuilder2D::BatchBuilder2D(
        std::shared_ptr<const GPUTexture> whiteTexture,
        std::size_t maxVertices, std::size_t maxIndices,
        std::size_t maxTextures
    )
    : whiteTexture_ (std::move(whiteTexture)),
      maxVertices_  (maxVertices),
      maxIndices_   (maxIndices),
      maxTextures_  (maxTextures)
    {
        // Even the largest supported primitive must fit an empty batch.
        if (!whiteTexture_ || maxVertices < CircleSegments * 4 || maxIndices < CircleSegments * 6 || maxTextures == 0)
        {
            throw std::invalid_argument("Invalid BatchBuilder2D capacity or white texture");
        }

        batch_.vertices.reserve(maxVertices);
        batch_.indices.reserve(maxIndices);
        batch_.textures.reserve(maxTextures);
    }

    bool BatchBuilder2D::append(
        std::span<const Vertex2D>                vertices,
        std::span<const std::uint32_t>           indices,
        const std::shared_ptr<const GPUTexture>& texture
    )
    {
        const auto found = std::ranges::find(batch_.textures, texture);
        const auto slot = static_cast<std::size_t>(found - batch_.textures.begin());

        if (
            batch_.vertices.size() + vertices.size() > maxVertices_ ||
            batch_.indices.size()  + indices.size()  > maxIndices_  ||
            (found == batch_.textures.end() && slot == maxTextures_)
        )
        {
            return false;
        }

        if (found == batch_.textures.end())
        {
            batch_.textures.push_back(texture);
        }

        const auto base = static_cast<std::uint32_t>(batch_.vertices.size());
        for (auto vertex : vertices)
        {
            vertex.textureSlot = static_cast<float>(slot);
            batch_.vertices.push_back(vertex);
        }

        for (auto index : indices)
        {
            batch_.indices.push_back(base + index);
        }

        return true;
    }

    bool BatchBuilder2D::tryAppendSprite(const std::shared_ptr<const GPUTexture>& texture, const math::Transform2D& transform, Colour tint)
    {
        std::array<Vertex2D, 4> vertices;
        const auto matrix = transform.matrix();

        for (std::size_t i = 0; i < Quad.size(); ++i)
        {
            vertices[i] = {transformPoint(matrix, Quad[i]), Quad[i], tint.toVec4(), 0};
        }

        return append(vertices, QuadIndices, texture);
    }

    bool BatchBuilder2D::tryAppendRect(const math::Transform2D& transform, Colour colour, bool fill)
    {
        std::array<math::Vec2, 4> points;
        const auto matrix = transform.matrix();

        for (std::size_t i = 0; i < Quad.size(); ++i)
        {
            points[i] = transformPoint(matrix, Quad[i]);
        }

        return appendPolygon(points, colour, fill);
    }

    bool BatchBuilder2D::tryAppendGlyph(
        const std::shared_ptr<const GPUTexture>& texture,
        math::Vec2 position,
        math::Vec2 size,
        math::Vec2 uvMin,
        math::Vec2 uvMax,
        Colour colour
    )
    {
        std::array<Vertex2D, 4> vertices;

        for (std::size_t i = 0; i < Quad.size(); ++i)
        {
            const auto corner = Quad[i];
            vertices[i] = {
                position + math::Vec2{corner.x * size.x, corner.y * size.y},
                {uvMin.x + corner.x * (uvMax.x - uvMin.x), uvMin.y + corner.y * (uvMax.y - uvMin.y)},
                colour.toVec4(),
                0
            };
        }

        return append(vertices, QuadIndices, texture);
    }

    bool BatchBuilder2D::tryAppendTriangle(const math::Transform2D& transform, Colour colour, bool fill)
    {
        const auto matrix = transform.matrix();

        const std::array<math::Vec2, 3> points = {
            transformPoint(matrix, {0, 1}),
            transformPoint(matrix, {1, 1}),
            transformPoint(matrix, {0.5f, 0})
        };

        return appendPolygon(points, colour, fill);
    }

    bool BatchBuilder2D::appendPolygon(std::span<const math::Vec2> points, Colour colour, bool fill)
    {
        std::array<Vertex2D,      CircleSegments * 4> vertices;
        std::array<std::uint32_t, CircleSegments * 6> indices;

        const auto count = points.size();

        std::size_t vertexCount = 0, indexCount = 0;

        if (fill)
        {
            for (auto point : points)
            {
                vertices[vertexCount++] = {point, {}, colour.toVec4(), 0};
            }

            for (std::uint32_t i = 1; i + 1 < count; ++i)
            {
                indices[indexCount++] = 0;
                indices[indexCount++] = i;
                indices[indexCount++] = i + 1;
            }
        }
        else
        {
            // A connected ring avoids overlapping translucent edge quads at joins.
            // Outlines are centred on the boundary and 1 world unit wide.
            for (std::size_t i = 0; i < count; ++i)
            {
                const auto incoming = (points[i] - points[(i + count - 1) % count]).normalized();
                const auto outgoing = (points[(i + 1) % count] - points[i]).normalized();

                const math::Vec2 before{ -incoming.y, incoming.x };
                const math::Vec2 after{  -outgoing.y, outgoing.x };

                const auto  miter  = (before + after).normalized();
                const float denominator = miter.dot(after);

                if (std::abs(denominator) < 0.0001f)
                {
                    return true; // Collapsed polygon.
                }

                const auto offset = miter * (0.5f / denominator);

                vertices[vertexCount++] = {points[i] + offset, {}, colour.toVec4(), 0};
                vertices[vertexCount++] = {points[i] - offset, {}, colour.toVec4(), 0};

                const auto base = static_cast<std::uint32_t>(i * 2);
                const auto next = static_cast<std::uint32_t>(((i + 1) % count) * 2);

                indices[indexCount++] = base;
                indices[indexCount++] = next;
                indices[indexCount++] = next + 1;
                indices[indexCount++] = next + 1;
                indices[indexCount++] = base + 1;
                indices[indexCount++] = base;
            }
        }

        return append({vertices.data(), vertexCount}, {indices.data(), indexCount}, whiteTexture_);
    }

    bool BatchBuilder2D::tryAppendLine(math::Vec2 from, math::Vec2 to, Colour colour, float thickness)
    {
        if (!(thickness > 0.0f) || (to - from).length() == 0.0f)
        {
            return true;
        }

        std::array<Vertex2D, 4> vertices;
        lineVertices(from, to, thickness, colour.toVec4(), vertices.data());

        return append(vertices, QuadIndices, whiteTexture_);
    }

    bool BatchBuilder2D::tryAppendCircle(math::Vec2 centre, float radius, Colour colour, bool fill)
    {
        if (!(radius > 0.0f) || !std::isfinite(radius))
        {
            return true;
        }

        std::array<math::Vec2, CircleSegments> points;
        for (std::size_t i = 0; i < CircleSegments; ++i)
        {
            const float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(CircleSegments);
            points[i]         = centre + math::Vec2{std::cos(angle), std::sin(angle)} * radius;
        }

        return appendPolygon(points, colour, fill);
    }
}
