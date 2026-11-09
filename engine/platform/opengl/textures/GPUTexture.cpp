#include <opengl/textures/GPUTexture.hpp>

#include <algorithm>
#include <bit>
#include <stdexcept>

#include <glad/gl.h>

#include <opengl/GLConversions.hpp>

#include <forge2d/assets/Image.hpp>
#include <forge2d/graphics/Texture2D.hpp>

namespace forge::internal
{
    GPUTexture::GPUTexture(const Image& image, const TextureOptions& options)
    {
        if (image.width <= 0 || image.height <= 0 || image.pixels.size() != static_cast<std::size_t>(image.width) * image.height * 4)
        {
            throw std::invalid_argument("GPUTexture requires positive dimensions and exact RGBA8 pixel data");
        }

        GLint maxSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
        
        if (image.width > maxSize || image.height > maxSize)
        {
            throw std::invalid_argument("GPUTexture exceeds GL_MAX_TEXTURE_SIZE");
        }

        // DSA: no glBindTexture needed to configure the texture.
        glCreateTextures(GL_TEXTURE_2D, 1, &id_);

        glTextureParameteri(id_, GL_TEXTURE_WRAP_S,     opengl::toGL(options.wrap));
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T,     opengl::toGL(options.wrap));
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, opengl::minFilter(options));
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, options.filter == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR);

        // bit_width(n) == floor(log2(n)) + 1 == full mip chain length
        const auto levels = options.mipmaps
            ? std::bit_width(static_cast<unsigned>(std::max(image.width, image.height)))
            : 1;

        glTextureStorage2D(id_, levels, GL_RGBA8, image.width, image.height);

        // Client image upload must not inherit another user's unpack buffer/layout.
        GLint unpackBuffer, alignment, rowLength, skipPixels, skipRows;
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &unpackBuffer);
        glGetIntegerv(GL_UNPACK_ALIGNMENT,            &alignment);
        glGetIntegerv(GL_UNPACK_ROW_LENGTH,           &rowLength);
        glGetIntegerv(GL_UNPACK_SKIP_PIXELS,          &skipPixels);
        glGetIntegerv(GL_UNPACK_SKIP_ROWS,            &skipRows);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        glPixelStorei(GL_UNPACK_ALIGNMENT,   1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH,  0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);

        glTextureSubImage2D(id_, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, static_cast<GLuint>(unpackBuffer));

        glPixelStorei(GL_UNPACK_ALIGNMENT,   alignment);
        glPixelStorei(GL_UNPACK_ROW_LENGTH,  rowLength);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, skipPixels);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,   skipRows);

        if (options.mipmaps)
        {
            glGenerateTextureMipmap(id_);
        }
    }

    GPUTexture::~GPUTexture()
    {
        if (id_ != 0)
        {
            glDeleteTextures(1, &id_);
        }
    }

    void GPUTexture::bind(const unsigned int slot) const
    {
        glBindTextureUnit(slot, id_);
    }
}
