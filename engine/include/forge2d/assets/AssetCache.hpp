#pragma once

/// @file AssetCache.hpp
/// @brief Engine-internal cache that `AssetManager` stores each asset type in.

#include <concepts>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

#include <forge2d/assets/AssetError.hpp>
#include <forge2d/debug/Log.hpp>

namespace forge::internal
{
    /// @brief Hash for `std::string` keys that also accepts `std::string_view`.
    ///
    /// Engine-internal. Together with `std::equal_to<>`, it lets an
    /// `unordered_map<std::string, ...>` be searched with a `string_view` without building a
    /// temporary `std::string` (C++20 heterogeneous lookup).
    struct StringHash
    {
        using is_transparent = void;

        std::size_t operator()(std::string_view s) const noexcept
        {
            return std::hash<std::string_view>{}(s);
        }
    };

    /// @brief A load-on-first-use cache for one asset type, with a fallback for failed loads.
    ///
    /// Engine-internal: games use `AssetManager`, which holds one cache per asset type.
    ///
    /// @tparam T The asset handle type. It must be a cheap, copyable handle (like `Texture2D`),
    ///           because every key that fails to load stores its own copy of the fallback.
    ///
    /// @note Not thread-safe. `AssetManager` only uses it from the main thread.
    template <typename T>
    class AssetCache
    {
        public:
            /// @brief Creates an empty cache.
            ///
            /// @param kind     Name of the asset type, used in log messages (e.g. `"texture"`).
            /// @param fallback Asset returned for keys that fail to load.
            AssetCache(std::string_view kind, T fallback) : kind_(kind), fallback_(std::move(fallback)) { }

            /// @brief Returns the asset cached under @p key, loading it first if needed.
            ///
            /// If @p key isn't cached yet, @p load is called and its result is cached. If @p load
            /// throws `AssetError`, a warning is logged and the fallback is cached under @p key
            /// instead, so a bad path is only tried (and logged) once. Other exceptions propagate
            /// and nothing is cached.
            ///
            /// @param key  Cache key. `AssetManager` normalises paths before using them as keys.
            /// @param load Called with no arguments to load the asset. Only called on a cache miss.
            /// @return The cached asset. The reference stays valid until `clear()` or the cache
            ///         is destroyed, because `unordered_map` never moves its elements.
            template <std::invocable Loader> requires std::convertible_to<std::invoke_result_t<Loader&>, T>
            const T& getOrLoad(std::string_view key, Loader&& load)
            {
                if (const auto it = entries_.find(key); it != entries_.end())
                {
                    return it->second;
                }

                T value = [&]() -> T {
                    try
                    {
                        return std::invoke(load);
                    }
                    catch (const AssetError& e)
                    {
                        FORGE_LOG_WARN("[AssetManager] could not load {}: {} ({})", kind_, key, e.what());
                        return fallback_;
                    }
                }();

                return entries_.emplace(std::string(key), std::move(value)).first->second;
            }

            /// @brief Whether @p key has been cached, including keys that fell back after failing.
            [[nodiscard]] bool contains(std::string_view key) const { return entries_.contains(key); }

            /// @brief The asset returned for keys that failed to load.
            [[nodiscard]] const T& fallback() const { return fallback_; }

            /// @brief Number of cached keys, including keys that fell back after failing.
            [[nodiscard]] std::size_t size()  const { return entries_.size(); }

            /// @brief Removes every cached entry. The fallback is kept.
            ///
            /// @warning Invalidates every reference returned by `getOrLoad()`.
            void clear() noexcept { entries_.clear(); }

        private:
            std::string kind_;
            T fallback_;

            std::unordered_map<std::string, T, StringHash, std::equal_to<>> entries_;
    };
}
