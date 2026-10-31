#pragma once

namespace forge
{
    struct Image;

    enum class TextureFilter { NEAREST, LINEAR };
    enum class TextureWrap   { REPEAT, CLAMP, MIRROR };

    struct TextureOptions
    {
        TextureFilter filter = TextureFilter::NEAREST; // pixel-art friendly default
        TextureWrap wrap     = TextureWrap::CLAMP;

        bool mipmaps = false;
    };

    /// A GPU texture. Move-only, owns its GL object.
    /// Must be created and destroyed on the thread that owns the GL context.
    class Texture2D
    {
        public:
            explicit Texture2D(const Image& image, const TextureOptions& options = {});

            ~Texture2D();

            Texture2D(const Texture2D&) = delete;
            Texture2D& operator=(const Texture2D&) = delete;

            Texture2D(Texture2D&& other) noexcept;
            Texture2D& operator=(Texture2D&& other) noexcept;

            [[nodiscard]] int width()  const noexcept { return width_; }
            [[nodiscard]] int height() const noexcept { return height_; }

            /// Engine internal (used by Renderer2D).
            void bind(unsigned int slot) const;

        private:
            unsigned int id_ = 0; // plain unsigned so this header needs no GL include

            int width_  = 0;
            int height_ = 0;
    };
}
