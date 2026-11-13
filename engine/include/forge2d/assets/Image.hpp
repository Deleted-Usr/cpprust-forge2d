#pragma once

/// @file Image.hpp
/// @brief Decoded images in CPU memory.

#include <cstdint>
#include <filesystem>
#include <vector>

namespace forge
{
    /// @brief A decoded image in CPU memory, ready to be uploaded with `Texture2D::create()`.
    ///
    /// Pixels are RGBA8 (4 bytes per pixel, straight alpha), stored row by row with the **top**
    /// row first, so `pixels` holds exactly `width * height * 4` bytes.
    ///
    /// An `Image` doesn't touch the GPU, so it can be created and loaded on any thread.
    /// `AssetManager::preloadTextures()` uses this to decode files on worker threads.
    ///
    /// Most games never use `Image` directly: `AssetManager::loadTexture()` loads and uploads in
    /// one step.
    ///
    /// @code
    /// // A 2x1 image: one red pixel, then one half-transparent white pixel.
    /// forge::Image image;
    /// image.width  = 2;
    /// image.height = 1;
    /// image.pixels = { 255, 0, 0, 255,   255, 255, 255, 128 };
    ///
    /// forge::Texture2D texture = forge::Texture2D::create(image);
    /// @endcode
    struct Image
    {
        int width  = 0; ///< Width in pixels.
        int height = 0; ///< Height in pixels.

        /// RGBA8 pixel data, top row first. Must hold `width * height * 4` bytes.
        std::vector<std::uint8_t> pixels;

        /// @brief Reads and decodes an image file, converting it to RGBA8.
        ///
        /// Decoding is done by the engine's Rust image decoder, which supports PNG and JPEG.
        ///
        /// @param path File to load. Relative paths are resolved against the current working
        ///             directory, not the asset root.
        /// @return The decoded image, with non-zero dimensions.
        ///
        /// @throws AssetError If the file can't be opened or read, can't be decoded, or has
        ///         dimensions that are zero or too large to store in an `int`.
        ///
        /// @note Thread-safe: it has no shared state.
        [[nodiscard]]
        static Image load(const std::filesystem::path& path);
    };
}
