#include "TestSupport.hpp"

#include <forge2d/assets/AssetError.hpp>
#include <forge2d/assets/Image.hpp>

#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

namespace fs = std::filesystem;

const fs::path fixtures = FORGE2D_IMAGE_FIXTURE_DIR;
const std::vector<std::uint8_t> expected{
    255, 0, 0, 255, 0, 255, 0, 64,
    0, 0, 255, 0, 255, 255, 255, 255
};

void expectFailure(const fs::path& path)
{
    try
    {
        (void)forge::Image::load(path);
    }
    catch (const forge::AssetError& error)
    {
        check(*error.what() != '\0', "failure includes diagnostic text");
        return;
    }
    throw std::runtime_error("Expected AssetError for " + path.string());
}

int main()
{
    try
    {
        const auto png = forge::Image::load(fixtures / "rgba.png");

        check(png.width == 2 && png.height == 2, "PNG dimensions");
        check(png.pixels == expected, "RGBA channels, alpha and top-row-first orientation");

        const auto gray = forge::Image::load(fixtures / "gray.png");

        check(gray.width  == 2 && gray.height == 1, "grayscale dimensions");
        check(gray.pixels == std::vector<std::uint8_t>{ 32, 32, 32, 255, 192, 192, 192, 255 }, "grayscale expands to RGBA8");

        const auto jpeg = forge::Image::load(fixtures / "red.jpg");
        check(jpeg.width         == 16 && jpeg.height == 16, "JPEG dimensions");
        check(jpeg.pixels.size() == 16 * 16 * 4,             "JPEG expands to RGBA8");

        for (std::size_t i = 0; i < jpeg.pixels.size(); i += 4)
        {
            check(jpeg.pixels[i] >= 240 && jpeg.pixels[i + 1] <= 15 &&
                  jpeg.pixels[i + 2] <= 15 && jpeg.pixels[i + 3] == 255,
                  "JPEG red pixels and opaque alpha"
            );
        }

        // Exercise existing repository assets as well as controlled fixtures.
        for (const auto* name : { "container.jpg", "heart16x16.png", "img.png" })
        {
            const auto image = forge::Image::load(fs::path(FORGE2D_ENGINE_ASSET_DIR) / name);

            check(image.width > 0 && image.height > 0, "repository asset dimensions");
            check(image.pixels.size() == static_cast<std::size_t>(image.width) * image.height * 4,"repository asset RGBA8 layout");
        }

        const fs::path scratch = FORGE2D_IMAGE_TEST_TEMP_DIR;
        const auto unicode = scratch / fs::path(u8"image-\u00e9-\u753b.png");

        fs::copy_file(fixtures / "rgba.png", unicode, fs::copy_options::overwrite_existing);
        const auto unicodeImage = forge::Image::load(unicode);
        fs::remove(unicode);

        check(unicodeImage.pixels == expected, "Unicode Windows path");

        expectFailure(fixtures / "does-not-exist.png");
        const auto invalid = scratch / "invalid-image.png";

        {
            std::ofstream file(invalid, std::ios::binary);
        }

        expectFailure(invalid);

        {
            std::ofstream file(invalid, std::ios::binary); file << "not an image";
        }

        expectFailure(invalid);

        {
            std::ifstream source(fixtures / "rgba.png", std::ios::binary);
            char header[24]{};

            source.read(header, sizeof(header));

            std::ofstream file(invalid, std::ios::binary);

            file.write(header, sizeof(header));
        }

        expectFailure(invalid);
        fs::remove(invalid);

        std::vector<std::future<forge::Image>> jobs;
        for (int i = 0; i < 8; ++i)
        {

            jobs.push_back(std::async(std::launch::async, [] {
                return forge::Image::load(fixtures / "rgba.png");
            }));
        }
        for (auto& job : jobs)
        {
            check(job.get().pixels == expected, "parallel decoding");
        }

        std::cout << "Image checks passed: " << checks << '\n';

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Image failure: " << error.what() << '\n';
        return 1;
    }
}
