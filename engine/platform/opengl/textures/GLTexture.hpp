#pragma once

namespace forge
{
    struct Image;
    struct TextureOptions;
}

namespace forge::internal
{
    /// Owns one OpenGL texture object. Engine-internal: users only see Texture2D.
    /// Not copyable or movable; it is always held through a shared_ptr.
    class GLTexture
    {
        public:
            GLTexture(const Image& image, const TextureOptions& options);
            ~GLTexture();

            GLTexture(const GLTexture&) = delete;
            GLTexture& operator=(const GLTexture&) = delete;

            void bind(unsigned int slot) const;

        private:
            unsigned int id_ = 0; // plain unsigned so this header needs no GL include
    };
}
