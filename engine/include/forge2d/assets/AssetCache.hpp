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
    template <typename T>
    class AssetCache
    {
        public:
            AssetCache(std::string_view kind, T fallback) : kind_(kind), fallback_(std::make_unique<const T>(std::move(fallback))) { }

            template <std::invocable Loader> requires std::convertible_to<std::invoke_result_t<Loader&>, T>
            const T& getOrLoad(std::string_view key, Loader&& load)
            {
                if (const auto it = entries_.find(key); it != entries_.end())
                {
                    return *it->second;
                }

                std::shared_ptr<const T> value;

                try
                {
                    value = std::make_shared<const T>(std::invoke(load));
                }
                catch (const AssetError& e)
                {
                    // TODO - route through the engine logger once there is one
                    std::cerr << "[AssetManager] could not load " << kind_ << ": " << key << " (" << e.what() << ")\n";
                    value = fallback_;
                }

                return *entries_.emplace(std::string(key), std::move(value)).first->second;
            }

            [[nodiscard]] bool contains(std::string_view key) const { return entries_.contains(key); }
            [[nodiscard]] const T& fallback() const { return *fallback_; }
            [[nodiscard]] std::size_t size()  const { return entries_.size(); }

            void clear() noexcept { entries_.clear(); }

        private:
            std::string kind_;
            std::shared_ptr<const T> fallback_;

            // shared_ptr so every failed key can point at the one fallback instance.
            std::unordered_map<std::string, std::shared_ptr<const T>, StringHash, std::equal_to<>> entries_;
    };
}
