#include <forge2d/graphics/Texture2D.hpp>

#include <utility>

#include <opengl/textures/GPUTexture.hpp>

#include <forge2d/assets/Image.hpp>
#include <forge2d/debug/Assert.hpp>

namespace forge
{
    Texture2D::Texture2D(std::shared_ptr<const internal::GPUTexture> gpu, const int width, const int height) noexcept : gpu_(std::move(gpu)), width_(width), height_(height) { }

    Texture2D Texture2D::create(const Image& image, const TextureOptions& options)
    {
        return { std::make_shared<const internal::GPUTexture>(image, options), image.width, image.height };
    }

}
