#include <forge2d/assets/Image.hpp>
#include <forge2d/assets/AssetError.hpp>

#include <rust/cxx.h>
#include <forge2d_rust/assets.h>

#include <fstream>
#include <iterator>
#include <limits>

namespace forge
{
    namespace
    {
        std::vector<std::uint8_t> readBytes(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary);

            if (!file)
            {
                throw AssetError("Could not open image file");
            }

            std::vector<std::uint8_t> bytes{
                std::istreambuf_iterator(file),
                std::istreambuf_iterator<char>()
            };

            if (file.bad())
            {
                throw AssetError("Could not read image file");
            }

            return bytes;
        }
    }

    Image Image::load(const std::filesystem::path& path)
    {
        const auto bytes = readBytes(path);

        try
        {
            // Leading :: selects CXX's namespace, rather than forge::rust.
            const auto decoded = rust::decode_image(
                ::rust::Slice(bytes.data(), bytes.size())
            );

            constexpr auto maxDimension =static_cast<std::uint32_t>(std::numeric_limits<int>::max());

            if (
                decoded.width  == 0           ||
                decoded.height == 0           ||
                decoded.width  > maxDimension ||
                decoded.height > maxDimension
            )
            {
                throw AssetError("Image dimensions are out of range");
            }

            Image image;
            image.width  = static_cast<int>(decoded.width);
            image.height = static_cast<int>(decoded.height);

            image.pixels.assign(
                decoded.pixels.begin(),
                decoded.pixels.end()
            );

            return image;
        }
        catch (const ::rust::Error& error)
        {
            // Preserve AssetManager's existing fallback behavior.
            throw AssetError(error.what());
        }
    }
}
