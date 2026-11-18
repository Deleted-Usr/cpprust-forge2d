#pragma once

/// @file AssetID.hpp
/// @brief Typed asset identifiers, so the wrong kind of asset can't be loaded by mistake.

#include <string_view>

namespace forge::assets
{
    /// @brief Identifies a texture file. Pass it to `AssetManager::loadTexture()`.
    ///
    /// Each asset kind has its own identifier type, so passing a `FontAsset` to `loadTexture()`
    /// is a compile error rather than a runtime surprise. Identifiers are cheap to copy and are
    /// designed to be declared as `constexpr` constants in one place:
    ///
    /// @code
    /// namespace textures
    /// {
    ///     inline constexpr forge::assets::TextureAsset HEART { "heart16x16.png" };
    ///     inline constexpr forge::assets::TextureAsset IDLE  { .path = "Player/Idle.png", .preload = false };
    /// }
    ///
    /// const forge::Texture2D& heart = assets.loadTexture(textures::HEART);
    /// @endcode
    struct TextureAsset
    {
        /// Path relative to the asset root (`ApplicationConfig::assetRoot`), using `/` separators.
        ///
        /// @warning Only a view is stored. Point it at a string literal or other storage that
        ///          outlives the identifier.
        std::string_view path;

        /// A hint for game code that builds a list to pass to `AssetManager::preloadTextures()`.
        /// The engine itself doesn't read it. Defaults to `true`.
        bool preload = true;
    };

    /// @brief Identifies a font file. Pass it to `AssetManager::loadFont()`.
    ///
    /// A font is loaded once and drawn at any size, so declare one identifier per font file,
    /// not per size.
    ///
    /// @code
    /// namespace fonts
    /// {
    ///     inline constexpr forge::assets::FontAsset UI { "fonts/MyFont.ttf" };
    /// }
    ///
    /// const forge::Font& ui = assets.loadFont(fonts::UI);
    /// @endcode
    struct FontAsset
    {
        /// Path relative to the asset root, using `/` separators. Only a view is stored, so it
        /// must outlive the identifier.
        std::string_view path;

        /// A hint for game code that builds preload lists. The engine itself doesn't read it.
        /// Defaults to `true`.
        bool preload = true;
    };
}
