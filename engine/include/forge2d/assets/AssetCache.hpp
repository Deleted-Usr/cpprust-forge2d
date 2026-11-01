#pragma once

#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <forge2d/assets/AssetError.hpp>

namespace forge::internal
{
    /// Lets unordered_map<std::string, ...> be searched with a string_view
    /// without building a temporary std::string (C++20 heterogeneous lookup).
    struct StringHash
    {
        using is_transparent = void;

        std::size_t operator()(std::string_view s) const noexcept
        {
            return std::hash<std::string_view>{}(s);
        }
    };

    /// Generic "computeIfAbsent" cache with a fallback for failed loads.
    /// One instance per asset type (textures now, fonts/sounds later).
    ///
    /// Convention: every asset type is a cheap, copyable handle (like Texture2D),
    /// so failed keys can simply hold a copy of the fallback handle.
    template <typename T>
    class AssetCache
    {
        public:
            AssetCache(std::string_view kind, T fallback) : kind_(kind), fallback_(std::move(fallback)) { }

            /// Returns the cached asset, or calls load() and caches the result.
            /// If load() throws AssetError, the failure is logged and the fallback is
            /// cached under this key, so a bad path is only attempted (and logged) once.
            ///
            /// The returned reference is stable: unordered_map never moves its
            /// elements, so it stays valid until clear() or the cache is destroyed.
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
                        // TODO: route through the engine logger once it exists.

                        std::cerr << "[AssetManager] could not load " << kind_ << ": " << key << " (" << e.what() << ")\n";
                        return fallback_;
                    }
                }();

                return entries_.emplace(std::string(key), std::move(value)).first->second;
            }

            [[nodiscard]] bool contains(std::string_view key) const { return entries_.contains(key); }
            [[nodiscard]] const T& fallback() const { return fallback_; }
            [[nodiscard]] std::size_t size()  const { return entries_.size(); }

            void clear() noexcept { entries_.clear(); }

        private:
            std::string kind_;
            T fallback_;

            std::unordered_map<std::string, T, StringHash, std::equal_to<>> entries_;
    };
}
