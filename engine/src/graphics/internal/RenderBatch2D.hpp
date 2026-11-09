#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Vec4.hpp>

namespace forge::internal
{
    class GPUTexture;

    struct Vertex2D
    {
        math::Vec2 position;
        math::Vec2 uv;
        math::Vec4 colour;
        float textureSlot;
    };

    struct RenderBatch2D
    {
        std::vector<Vertex2D> vertices;
        std::vector<std::uint32_t> indices;

        // Retain resources until this batch has been submitted.
        std::vector<std::shared_ptr<const GPUTexture>> textures;

        void clear()
        {
            vertices.clear();
            indices.clear();
            textures.clear();
        }

        [[nodiscard]] bool empty() const
        {
            return indices.empty();
        }
    };
}
