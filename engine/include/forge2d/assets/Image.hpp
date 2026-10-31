#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace forge
{
    /// Decoded image in CPU memory: RGBA8, row-major, top row first.
    /// Has no GPU dependency, so it is safe to create on any thread.
    struct Image
    {
        int width  = 0;
        int height = 0;

        std::vector<std::uint8_t> pixels;

        /// Throws AssetError if the file can't be read or decoded.
        [[nodiscard]]
        static Image load(const std::filesystem::path& path);
    };
}
