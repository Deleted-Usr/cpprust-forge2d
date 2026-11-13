#pragma once

/// @file AssetError.hpp
/// @brief The exception asset loaders throw when a file can't be loaded.

#include <stdexcept>

namespace forge
{
    /// @brief Thrown when an asset can't be read or decoded.
    ///
    /// `Image::load()` throws it for missing, unreadable or corrupt files. `AssetManager` catches
    /// it, logs a warning and substitutes a fallback asset, so games using `AssetManager` never
    /// see it. Only code that calls loaders such as `Image::load()` directly needs to handle it.
    ///
    /// @code
    /// try
    /// {
    ///     forge::Image icon = forge::Image::load("assets/icon.png");
    /// }
    /// catch (const forge::AssetError& e)
    /// {
    ///     FORGE_LOG_WARN("no icon: {}", e.what());
    /// }
    /// @endcode
    class AssetError : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
}
