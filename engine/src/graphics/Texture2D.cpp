#include <forge2d/graphics/Texture2D.hpp>

#include <utility>

#include <opengl/textures/GLTexture.hpp>

#include <forge2d/assets/Image.hpp>
#include <forge2d/debug/Assert.hpp>

namespace forge
{
    Texture2D::Texture2D(std::shared_ptr<const internal::GLTexture> gpu, const int width, const int height) noexcept : gpu_(std::move(gpu)), width_(width), height_(height) { }

    Texture2D Texture2D::create(const Image& image, const TextureOptions& options)
    {
        return { std::make_shared<const internal::GLTexture>(image, options), image.width, image.height };
    }

    void Texture2D::bind(const unsigned int slot) const
    {
        FORGE_ASSERT(valid(), "Drawing an empty Texture2D: did you forget to assign it from loadTexture()?");

        gpu_->bind(slot);
    }
}
