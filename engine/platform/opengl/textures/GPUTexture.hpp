#pragma once
#include <cstdint>
#include <span>

namespace forge
{
    struct Image;
    struct TextureOptions;
}

namespace forge::internal
{
    /// Owns one OpenGL texture object. Engine-internal: users only see Texture2D.
    /// Not copyable or movable; it is always held through a shared_ptr.
    class GPUTexture
    {
        public:
            GPUTexture(const Image& image, const TextureOptions& options);
            /// Zero-initialised R8 coverage page; sampled as white RGB + coverage alpha.
            GPUTexture(int width, int height);
            void uploadCoverage(int x, int y, int width, int height,
                                std::span<const std::uint8_t> pixels) const;
            ~GPUTexture();

            GPUTexture(const GPUTexture&) = delete;
            GPUTexture& operator=(const GPUTexture&) = delete;

            void bind(unsigned int slot) const;

        private:
            unsigned int id_ = 0; // plain unsigned so this header needs no GL include
            int width_ = 0, height_ = 0;
            bool coverage_ = false;
    };
}
