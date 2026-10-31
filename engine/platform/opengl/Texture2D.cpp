#include <forge2d/graphics/Texture2D.hpp>
#include <forge2d/assets/Image.hpp>

#include <algorithm>
#include <bit>
#include <utility>

#include <glad/gl.h>

namespace forge
{
    namespace
    {
        GLint toGL(const TextureWrap wrap)
        {
            switch (wrap)
            {
                case TextureWrap::CLAMP:  return GL_CLAMP_TO_EDGE;
                case TextureWrap::REPEAT: return GL_REPEAT;
                case TextureWrap::MIRROR: return GL_MIRRORED_REPEAT;
            }

            return GL_CLAMP_TO_EDGE;
        }

        GLint minFilter(const TextureOptions& options)
        {
            const bool nearest = options.filter == TextureFilter::NEAREST;

            if (!options.mipmaps)
            {
                return nearest ? GL_NEAREST : GL_LINEAR;
            }

            return nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
        }
    }

    Texture2D::Texture2D(const Image& image, const TextureOptions& options) : width_(image.width), height_(image.height)
    {
        // DSA: no glBindTexture needed to configure the texture.
        glCreateTextures(GL_TEXTURE_2D, 1, &id_);

        glTextureParameteri(id_, GL_TEXTURE_WRAP_S,     toGL(options.wrap));
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T,     toGL(options.wrap));
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, minFilter(options));
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, options.filter == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR);

        // bit_width(n) == floor(log2(n)) + 1 == full mip chain length
        const auto levels = options.mipmaps
            ? std::bit_width(static_cast<unsigned>(std::max(width_, height_)))
            : 1;

        glTextureStorage2D(id_, levels, GL_RGBA8, width_, height_);
        glTextureSubImage2D(id_, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());

        if (options.mipmaps)
        {
            glGenerateTextureMipmap(id_);
        }
    }

    Texture2D::~Texture2D()
    {
        if (id_ != 0)
        {
            glDeleteTextures(1, &id_);
        }
    }

    Texture2D::Texture2D(Texture2D&& other) noexcept : id_(std::exchange(other.id_, 0)), width_(other.width_), height_(other.height_) { }

    Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
    {
        if (this != &other)
        {
            if (id_ != 0)
            {
                glDeleteTextures(1, &id_);
            }

            id_     = std::exchange(other.id_, 0);
            width_  = other.width_;
            height_ = other.height_;
        }

        return *this;
    }

    void Texture2D::bind(const unsigned int slot) const
    {
        glBindTextureUnit(slot, id_);
    }
}
