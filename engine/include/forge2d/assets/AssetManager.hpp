#pragma once

/// @file AssetManager.hpp
/// @brief Loads and caches the assets a game uses.

#include <concepts>
#include <filesystem>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <forge2d/assets/AssetID.hpp>
#include <forge2d/graphics/Texture2D.hpp>

namespace forge
{
    /// @brief A list of texture handles, as returned by `AssetManager::loadTextureRange()`.
    using TextureList = std::vector<Texture2D>;

    /// @brief Loads assets from disk and caches them, so each file is only loaded once.
    ///
    /// `Application` owns the asset manager and passes it to `Game::onStart()`. It isn't a
    /// singleton: its lifetime is tied to the window's GL context, which its textures need.
    ///
    /// ### Paths
    /// Paths are relative to the asset root (`ApplicationConfig::assetRoot`) and are
    /// normalised before caching, so `"a/../b.png"`, `"./b.png"` and `"b.png"` all refer to the
    /// same cached texture. Use `/` as the separator on every platform.
    ///
    /// ### Missing assets
    /// A file that can't be read or decoded is never fatal. A warning is logged once and the
    /// path is given the **missing texture**: a 40×40 magenta square with a black border, so the
    /// problem is obvious on screen. Later loads of the same path return it without retrying.
    ///
    /// ### Lifetime
    /// `loadTexture()` returns a reference to the cached handle, so callers can either:
    /// - copy it, which keeps the GPU texture alive on its own and can't dangle; or
    /// - keep a pointer or reference to it, which stays valid until the `AssetManager` is
    ///   destroyed.
    ///
    /// `Application` destroys the game before the asset manager, so both are safe for
    /// members of a `Game`.
    ///
    /// ### Threading
    /// Call it from the main (GL) thread only (Debug builds assert). `preloadTextures()` uses
    /// worker threads internally, but still has to be called from the main thread.
    ///
    /// @code
    /// void onStart(forge::AssetManager& assets) override
    /// {
    ///     heart_ = assets.loadTexture("heart16x16.png");          // copy
    ///     coin_  = &assets.loadTexture(textures::COIN_SPIN);      // pointer
    ///     grass_ = assets.loadTextureRange([](int i) { return std::format("Tiles/Grass{}.png", i); }, 4);
    /// }
    /// @endcode
    class AssetManager
    {
        // The value version is actually the safer one: a copy keeps its texture alive on its own, so it can’t dangle.
        // The pointer is non-owning and valid only while the AssetManager exists. Because Application destroys the
        // game before the asset manager, both styles are safe in normal use.
        public:
            /// @brief Creates an asset manager that loads files from @p assetRoot.
            ///
            /// Also uploads the missing texture, so it can be returned at any time.
            ///
            /// @param assetRoot Folder that asset paths are relative to. A relative root is
            ///                  resolved against the current working directory.
            ///
            /// @pre Called on the main (GL) thread, with the engine's GL context current.
            explicit AssetManager(std::filesystem::path assetRoot);

            /// @brief Releases every cached asset.
            ///
            /// GPU textures still referenced by copied `Texture2D` handles stay alive until those
            /// handles are destroyed.
            ///
            /// @pre The GL context that created the textures still exists.
            ~AssetManager();

            AssetManager(const AssetManager&) = delete;
            AssetManager& operator=(const AssetManager&) = delete;

            /// @brief Loads a texture by its typed identifier, or returns it from the cache.
            ///
            /// Preferred over the string overload: the identifier is checked at compile time and
            /// keeps paths in one place.
            ///
            /// @param asset The texture to load. Only `asset.path` is used.
            /// @return The cached handle, or the missing texture if the file couldn't be loaded.
            ///         See the class documentation for how long the reference stays valid.
            ///
            /// @pre Called on the main (GL) thread.
            /// @throws std::invalid_argument If the image is larger than the GPU's maximum
            ///         texture size (see `Texture2D::create()`).
            const Texture2D& loadTexture(assets::TextureAsset asset);

            /// @brief Loads a texture by path, or returns it from the cache.
            ///
            /// Use it for paths built at run time, such as numbered tile variants.
            ///
            /// @param path Path relative to the asset root.
            /// @return The cached handle, or the missing texture if the file couldn't be loaded.
            ///
            /// @pre Called on the main (GL) thread.
            /// @throws std::invalid_argument If the image is larger than the GPU's maximum
            ///         texture size (see `Texture2D::create()`).
            const Texture2D& loadTexture(std::string_view path);

            /// @brief Loads a numbered family of textures: `pathFn(0)` up to `pathFn(count - 1)`.
            ///
            /// Each path is loaded with `loadTexture()`, so cached paths aren't loaded again and
            /// missing files become the missing texture.
            ///
            /// @param pathFn Called with each index from `0` to `count - 1`, returning the path
            ///               for that index (anything convertible to `std::string_view`, such as a
            ///               `std::string`).
            /// @param count  Number of textures to load. Zero or negative returns an empty list.
            /// @return Copies of the handles, in index order.
            ///
            /// @code
            /// forge::TextureList grass = assets.loadTextureRange(
            ///     [](int i) { return std::format("Tiles/Grass{}.png", i); }, 4);
            /// @endcode
            ///
            /// @pre Called on the main (GL) thread.
            template <typename PathFn> requires std::invocable<PathFn&, int> && std::convertible_to<std::invoke_result_t<PathFn&, int>, std::string_view>
            TextureList loadTextureRange(PathFn&& pathFn, const int count)
            {
                TextureList out;

                if (count <= 0)
                {
                    return out;
                }

                out.reserve(static_cast<std::size_t>(count));

                for (int i = 0; i < count; i++)
                {
                    const auto path = std::invoke(pathFn, i); // keep the string alive for the call

                    // Was emplace_back(..) because the vector held std::reference_wrappers
                    // so a new reference_wrapper had to be constructed from that argument
                    // and constructing an element from arguments in place is what emplace_back
                    // is for.
                    //
                    // Emplace_back can also call explicit contructors, but push_back only allows
                    // implicit conversions, so emplace_back can hide mistakes that push_back would
                    // catch.
                    //
                    // emplace_back(constructor arguments)
                    // push_back(object)
                    //
                    // Is now push_back to show proper code intent. (emplace_back would provide the same result)
                    out.push_back(loadTexture(std::string_view(path)));
                }

                return out;
            }

            /// @brief Loads many textures at once, decoding them in parallel.
            ///
            /// Files are decoded on worker threads, then uploaded to the GPU on the calling thread.
            /// This is much faster than calling `loadTexture()` for each path, so use it while a
            /// level or the game is starting up. Afterwards, `loadTexture()` returns the preloaded
            /// textures straight from the cache.
            ///
            /// Paths that are already cached, or repeated in @p paths, are skipped, so it's safe
            /// to call more than once. Files that fail to load are handled the same way as in
            /// `loadTexture()`.
            ///
            /// @param paths Paths relative to the asset root.
            ///
            /// @pre Called on the main (GL) thread. Blocks until every texture is uploaded.
            void preloadTextures(std::span<const std::string> paths);

            /// @brief The texture given to paths that failed to load: a 40×40 magenta square with
            ///        a black border.
            ///
            /// Compare against it to detect missing assets:
            /// `if (tex == assets.missingTexture()) { ... }`.
            [[nodiscard]] const Texture2D& missingTexture() const;

            /// @brief Number of cached texture paths, including paths that fell back to the
            ///        missing texture. The missing texture itself isn't counted.
            [[nodiscard]] std::size_t textureCount()        const;

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
    };
}
