#pragma once

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
    using TextureList = std::vector<Texture2D>;

    /// Owns and caches every loaded asset. Not a singleton: Application owns one
    /// and hands it to the game, so its lifetime is tied to the GL context.
    ///
    /// Threading: call it from the main (GL) thread only. preloadTextures() decodes
    /// files on worker threads internally and uploads them on the calling thread.
    ///
    /// loadTexture() returns a reference to the cached handle, so callers can either:
    ///  - copy it:     forge::Texture2D heart = assets.loadTexture(...);
    ///  - point at it: const forge::Texture2D* heart = &assetManager.loadTexture(...);
    ///
    /// A copy keeps the GPU texture alive by itself. A pointer/reference stays valid
    /// until the AssetManager is destroyed.
    class AssetManager
    {
        public:
            explicit AssetManager(std::filesystem::path assetRoot);
            ~AssetManager();

            AssetManager(const AssetManager&) = delete;
            AssetManager& operator=(const AssetManager&) = delete;

            /// Typed overload (preferred: compile-checked, no magic strings)
            const Texture2D& loadTexture(assets::TextureAsset asset);

            /// String overload (for generated paths, e.g. indexed tile families)
            const Texture2D& loadTexture(std::string_view path);

            // Loads pathFn(0) .. pathFn(count - 1). Equivalent of the Java loadRange().
            template <typename PathFn> requires std::invocable<PathFn&, int> && std::convertible_to<std::invoke_result_t<PathFn&, int>, std::string_view>
            TextureList loadTextureRange(PathFn&& pathFn, const int count)
            {
                TextureList out;
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

            /// Decodes every not-yet cached path in parallel, then uploads them to the GPU.
            /// Safe to call more than once; cached paths are skipped.
            void preloadTextures(std::span<const std::string> paths);

            [[nodiscard]] const Texture2D& missingTexture() const;
            [[nodiscard]] std::size_t textureCount()        const;

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
    };
}
