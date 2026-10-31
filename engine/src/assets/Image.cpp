#include <forge2d/assets/Image.hpp>
#include <forge2d/assets/AssetError.hpp>

#include <climits>
#include <fstream>
#include <iterator>
#include <memory>

// Declarations only. The implementation is compiled once, in stb_image.cpp.
#include <stb/stb_image.h>

namespace forge
{
    namespace
    {
        std::vector<stbi_uc> readBytes(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary);

            if (!file)
            {
                throw AssetError("Could not open file!");
            }

            return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
        }

        struct StbiDeleter
        {
            void operator()(stbi_uc* data) const noexcept { stbi_image_free(data); }
        };
    }

    Image Image::load(const std::filesystem::path& path)
    {
        // Reading the file manually (instead of stbi_load(path.c_str())) handles
        // non-ASCII Windows paths, and is the hook point for a future VFS/pak reader.
        const std::vector<stbi_uc> bytes = readBytes(path);

        if (bytes.size() > static_cast<std::size_t>(INT_MAX))
        {
            throw AssetError("File is too large!");
        }

        int width = 0, height = 0, channels = 0;

        // Always ask for 4 channels so every texture is uniformly RGBA8
        const std::unique_ptr<stbi_uc, StbiDeleter> data(
            stbi_load_from_memory(
                bytes.data(),
                static_cast<int>(bytes.size()),
                &width,
                &height,
                &channels,
                STBI_rgb_alpha
            )
        );

        if (!data)
        {
            throw AssetError(stbi_failure_reason());
        }

        Image image;
        image.width  = width;
        image.height = height;
        image.pixels.assign(data.get(), data.get() + static_cast<std::size_t>(width * height * 4));

        return image;
    }
}
