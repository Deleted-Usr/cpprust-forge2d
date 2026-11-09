#pragma once

#include <memory>

namespace forge
{
    struct Image;

    namespace internal
    {
        class GPUTexture;
        struct TextureAccess;
    }

    enum class TextureFilter { NEAREST, LINEAR };
    enum class TextureWrap   { REPEAT, CLAMP, MIRROR };

    struct TextureOptions
    {
        TextureFilter filter = TextureFilter::NEAREST; // pixel-art friendly default
        TextureWrap wrap     = TextureWrap::CLAMP;

        bool mipmaps = false;
    };

    /// A lightweight, copyable handle to a GPU texture.
    ///
    /// Copying a Texture2D does NOT copy pixels or create a new GPU texture; every
    /// copy refers to the same one (reference counted). The GPU texture is freed
    /// when the last handle referring to it is destroyed.
    ///
    /// A default-constructed Texture2D is empty (valid() == false) until assigned.
    ///
    /// Don't store Texture2D in globals/statics: a handle must not outlive the window,
    /// because the GPU texture can only be freed while the GL context exists.
    class Texture2D
    {
        public:
            Texture2D() = default;

            /// Uploads an image to the GPU. Must be called on the GL thread.
            [[nodiscard]] static Texture2D create(const Image& image, const TextureOptions& options = {});

            [[nodiscard]] int width()  const noexcept { return width_; }
            [[nodiscard]] int height() const noexcept { return height_; }

            [[nodiscard]] bool valid() const noexcept { return gpu_ != nullptr; }
            explicit operator bool()   const noexcept { return valid(); }

            /// True if both handles refer to the same GPU texture.
            friend bool operator==(const Texture2D&, const Texture2D&) = default;

        private:
            friend struct internal::TextureAccess;
            Texture2D(std::shared_ptr<const internal::GPUTexture> gpu, int width, int height) noexcept;

            std::shared_ptr<const internal::GPUTexture> gpu_;

            int width_  = 0;
            int height_ = 0;
    };
}
