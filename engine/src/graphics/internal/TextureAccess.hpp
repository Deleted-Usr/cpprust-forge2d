#pragma once

#include <forge2d/graphics/Texture2D.hpp>

namespace forge::internal
{
    /// Private bridge: public texture handles expose no GPU operations.
    struct TextureAccess
    {
        static std::shared_ptr<const GPUTexture> resource(const Texture2D& texture)
        {
            return texture.gpu_;
        }
    };
}
