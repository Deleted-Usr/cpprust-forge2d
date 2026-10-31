#include <forge2d/assets/AssetManager.hpp>

#include <cassert>
#include <future>
#include <thread>
#include <unordered_set>

#include <forge2d/assets/Image.hpp>
#include <forge2d/graphics/Texture2D.hpp>

#include <forge2d/assets/AssetCache.hpp>

namespace forge
{
    namespace
    {
        /// Visible magenta placeholder (black border) so a missing asset shows up
        /// on screen instead of silently not rendering.
        Image makeMissingTexture()
        {
            constexpr int size = 40;

            Image image;
            image.width  = size;
            image.height = size;
            image.pixels.resize(static_cast<std::size_t>(size) * size * 4);

            for (int y = 0; y < size; ++y)
            {
                for (int x = 0; x < size; ++x)
                {
                    const bool border = x == 0 || y == 0 || x == size - 1 || y == size - 1;
                    std::uint8_t* px  = &image.pixels[(static_cast<std::size_t>(y) * size + x) * 4];

                    px[0] = border ? 0 : 255; // R
                    px[1] = 0;                // G
                    px[2] = border ? 0 : 255; // B
                    px[3] = 255;              // A
                }
            }

            return image;
        }

        // "a/../b.png", "./b.png" and "b.png" all become the same cache key.
        std::string normaliseKey(std::string_view path)
        {
            return std::filesystem::path(path).lexically_normal().generic_string();
        }
    }

    struct AssetManager::Impl
    {
        std::filesystem::path root;
        std::thread::id owner = std::this_thread::get_id();

        internal::AssetCache<Texture2D> textures;

        explicit Impl(std::filesystem::path assetRoot) : root(std::move(assetRoot)), textures("texture", Texture2D(makeMissingTexture())) { }

        void assertOwnerThread() const
        {
            assert(std::this_thread::get_id() == owner && "AssetManager must be from the GL thread");
        }

        [[nodiscard]]
        std::filesystem::path resolve(std::string_view key) const
        {
            return root / key;
        }
    };

    AssetManager::AssetManager(std::filesystem::path assetRoot) : impl_(std::make_unique<Impl>(std::move(assetRoot))) { }
    AssetManager::~AssetManager() = default;

    const Texture2D& AssetManager::loadTexture(const assets::TextureAsset asset)
    {
        return loadTexture(asset.path);
    }

    const Texture2D& AssetManager::loadTexture(const std::string_view path)
    {
        impl_->assertOwnerThread();

        const std::string key = normaliseKey(path);

        return impl_->textures.getOrLoad(key, [&] {
            return Texture2D(Image::load(impl_->resolve(key)));
        });
    }

    void AssetManager::preloadTextures(const std::span<const std::string> paths)
    {
        impl_->assertOwnerThread();

        struct Job
        {
            std::string key;
            std::future<Image> image;
        };

        std::vector<Job> jobs;
        std::unordered_set<std::string> queued;

        // 1. Decode on worker threads (pure CPU work, no GL)
        for (const std::string& path : paths)
        {
            std::string key = normaliseKey(path);

            if (impl_->textures.contains(key) || !queued.insert(key).second)
            {
                continue;
            }

            std::filesystem::path file = impl_->resolve(key);

            // TODO: replace with a job system / thread pool; one thread per file is fine for now.
            jobs.push_back({
                std::move(key),
                std::async(std::launch::async, [file = std::move(file)] { return Image::load(file); })
            });
        }

        // 2. Upload on this (GL) thread. future::get() rethrows any AssetError,
        //    which getOrLoad turns into the missing texture, same as loadTexture().
        for (Job& job : jobs)
        {
            impl_->textures.getOrLoad(job.key, [&] {
                return Texture2D(job.image.get());
            });
        }
    }

    const Texture2D& AssetManager::missingTexture() const
    {
        return impl_->textures.fallback();
    }

    std::size_t AssetManager::textureCount() const
    {
        return impl_->textures.size();
    }
}
