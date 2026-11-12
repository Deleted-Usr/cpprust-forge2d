#pragma once

/// @file Texture2D.hpp
/// @brief Copyable handles to GPU textures, and the options used to create them.

#include <memory>

namespace forge
{
    struct Image;

    namespace internal
    {
        class GPUTexture;
        struct TextureAccess;
    }

    /// @brief How a texture is sampled when drawn larger or smaller than its real size.
    enum class TextureFilter
    {
        NEAREST, ///< Picks the closest texel, keeping hard pixel edges. Best for pixel art.
        LINEAR   ///< Blends neighbouring texels, for smooth scaling. Best for high-resolution art.
    };

    /// @brief What happens when texture coordinates fall outside the texture.
    ///
    /// `Renderer2D::drawSprite()` always maps the texture exactly once onto the sprite, so this
    /// mainly matters at the very edges of a sprite when `LINEAR` filtering blends in texels from
    /// the other side.
    enum class TextureWrap
    {
        REPEAT, ///< Tiles the texture.
        CLAMP,  ///< Stretches the edge texels outwards.
        MIRROR  ///< Tiles the texture, mirroring every other copy.
    };

    /// @brief Settings used when a texture is uploaded to the GPU.
    struct TextureOptions
    {
        /// Sampling filter. Defaults to `NEAREST`, which suits pixel art.
        TextureFilter filter = TextureFilter::NEAREST;

        /// Behaviour outside the texture's edges. Defaults to `CLAMP`.
        TextureWrap wrap     = TextureWrap::CLAMP;

        /// Generates a full mipmap chain, which reduces shimmering when the texture is drawn much
        /// smaller than its real size. Costs about a third more GPU memory.
        bool mipmaps = false;
    };

    /// @brief A lightweight, copyable handle to a texture on the GPU.
    ///
    /// Copying a `Texture2D` does **not** copy pixels or create a new GPU texture: every copy
    /// refers to the same one, which is reference counted. The GPU texture is freed when the last
    /// handle referring to it is destroyed, so a copy keeps its texture alive on its own.
    ///
    /// A default-constructed `Texture2D` is empty (`valid() == false`) until something is
    /// assigned to it. Drawing an empty texture draws nothing.
    ///
    /// Most games get textures from `AssetManager::loadTexture()` instead of calling `create()`.
    ///
    /// @code
    /// forge::Texture2D heart = assets.loadTexture("heart16x16.png"); // copy: shares the GPU texture
    /// if (heart)
    /// {
    ///     renderer.drawSprite(heart, { .position = { 10, 10 }, .scale = { 32, 32 } });
    /// }
    /// @endcode
    ///
    /// @warning Don't store `Texture2D` in globals or statics. A handle must not outlive the
    ///          window, because the GPU texture can only be freed while the GL context exists.
    class Texture2D
    {
        public:
            /// @brief Creates an empty handle (`valid() == false`).
            Texture2D() = default;

            /// @brief Uploads an image to the GPU and returns a handle to it.
            ///
            /// @param image   RGBA8 pixel data, with exactly `width * height * 4` bytes.
            /// @param options Filtering, wrapping and mipmap settings.
            /// @return A valid handle to the new texture.
            ///
            /// @pre Called on the main (GL) thread, with the engine's GL context current.
            /// @throws std::invalid_argument If the image has zero or negative dimensions, its
            ///         pixel data is the wrong size, or it is larger than the GPU's maximum
            ///         texture size.
            [[nodiscard]] static Texture2D create(const Image& image, const TextureOptions& options = {});

            /// @brief Width in pixels, or `0` for an empty handle.
            [[nodiscard]] int width()  const noexcept { return width_; }

            /// @brief Height in pixels, or `0` for an empty handle.
            [[nodiscard]] int height() const noexcept { return height_; }

            /// @brief Whether this handle refers to a GPU texture.
            [[nodiscard]] bool valid() const noexcept { return gpu_ != nullptr; }

            /// @brief Same as `valid()`, so a handle can be used in an `if`.
            explicit operator bool()   const noexcept { return valid(); }

            /// @brief True if both handles refer to the same GPU texture. Two empty handles are
            ///        equal.
            friend bool operator==(const Texture2D&, const Texture2D&) = default;

        private:
            friend struct internal::TextureAccess;
            Texture2D(std::shared_ptr<const internal::GPUTexture> gpu, int width, int height) noexcept;

            std::shared_ptr<const internal::GPUTexture> gpu_;

            int width_  = 0;
            int height_ = 0;
    };
}