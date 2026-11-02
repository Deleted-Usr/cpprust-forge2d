#pragma once

#include <array>
#include <string>
#include <string_view>

#include <forge2d/assets/AssetId.hpp>
#include <forge2d/assets/AssetManager.hpp>

/// Central registry of every asset path in the platformer.
/// Paths are relative to the AssetManager's root ("assets/").
namespace platformer::assets
{
    using forge::assets::FontAsset;
    using forge::assets::TextureAsset;

    namespace font
    {
        inline constexpr FontAsset PIXEL { "pixelfont.ttf" };
    }

    namespace ui
    {
        inline constexpr TextureAsset HEART       { "heart16x16.png" };
        inline constexpr TextureAsset HEART_EMPTY { "heart_depleted16x16.png" };
    }

    namespace player
    {
        inline constexpr TextureAsset IDLE { .path = "Characters/Player/PlayerIdle.png", .preload = false };
        inline constexpr TextureAsset WALK { .path = "Characters/Player/PlayerWalk.png", .preload = false };
        inline constexpr TextureAsset JUMP { .path = "Characters/Player/PlayerJump.png", .preload = false };

        inline constexpr TextureAsset ARMS             { "Characters/Player/ArmSheets/PlayerArms.png" };
        inline constexpr TextureAsset ARMS_SWORD_SWING { "Characters/Player/ArmSheets/PlayerArms_SwordSwing.png" };
    }

    namespace enemy
    {
        inline constexpr TextureAsset IDLE { "Characters/Enemy/EnemyIdle.png" };
        inline constexpr TextureAsset WALK { "Characters/Enemy/EnemyWalk.png" };
        inline constexpr TextureAsset DIE  { "Characters/Enemy/EnemyDeath.png" };
    }

    namespace pickups
    {
        inline constexpr TextureAsset COIN_SPIN { "Collectables/Animations/CoinSpin16x16.png" };
    }

    // C++ has no Enum.values(), so every one-off asset must also be listed here
    // for preloadAll(). Forgetting one isn't an error; it just loads lazily on first use.
    inline constexpr std::array kAllTextures {
        ui::HEART, ui::HEART_EMPTY,
        player::IDLE, player::WALK, player::JUMP, player::ARMS, player::ARMS_SWORD_SWING,
        enemy::IDLE, enemy::WALK, enemy::DIE,
        pickups::COIN_SPIN,
    };

    // --- Indexed terrain families ---
    namespace terrain
    {
        inline constexpr int GRASS_VARIANTS = 3;
        inline constexpr int DIRT_VARIANTS  = 3;
        inline constexpr int ROCK_VARIANTS  = 5;

        // Ore is named by colour rather than number.
        inline constexpr std::array<std::string_view, 5> ORE_NAMES { "Blue", "Green", "Magenta", "Red", "Yellow" };
        inline constexpr int ORE_VARIANTS = static_cast<int>(ORE_NAMES.size());

        std::string grass(int i);
        std::string dirt(int i);
        std::string rock(int i);
        std::string ore(int i);

        forge::TextureList grassTiles(forge::AssetManager& assets);
        forge::TextureList dirtTiles(forge::AssetManager& assets);
        forge::TextureList rockTiles(forge::AssetManager& assets);
        forge::TextureList oreTiles(forge::AssetManager& assets);
    }

    // Loads every preload-flagged asset plus all terrain families up front.
    void preloadAll(forge::AssetManager& assets);
}