#include "Assets.hpp"

#include <format>
#include <vector>

namespace platformer::assets
{
    namespace terrain
    {
        std::string grass(const int i) { return std::format("Terrain/Grass/grass{}.png", i); }
        std::string dirt(const int i)  { return std::format("Terrain/Dirt/dirt{}.png", i); }
        std::string rock(const int i)  { return std::format("Terrain/Rock/rock{}.png", i); }

        std::string ore(const int i)
        {
            return std::format("Terrain/Ore/{}Ore.png", ORE_NAMES.at(static_cast<std::size_t>(i)));
        }

        forge::TextureList grassTiles(forge::AssetManager& assets) { return assets.loadTextureRange(grass, GRASS_VARIANTS); }
        forge::TextureList dirtTiles(forge::AssetManager& assets)  { return assets.loadTextureRange(dirt,  DIRT_VARIANTS); }
        forge::TextureList rockTiles(forge::AssetManager& assets)  { return assets.loadTextureRange(rock,  ROCK_VARIANTS); }
        forge::TextureList oreTiles(forge::AssetManager& assets)   { return assets.loadTextureRange(ore,   ORE_VARIANTS); }
    }

    void preloadAll(forge::AssetManager& assets)
    {
        std::vector<std::string> paths;

        for (const TextureAsset& texture : kAllTextures)
        {
            if (texture.preload)
            {
                paths.emplace_back(texture.path);
            }
        }

        const auto addFamily = [&paths](auto pathFn, const int count) {
            for (int i = 0; i < count; ++i)
            {
                paths.push_back(pathFn(i));
            }
        };

        addFamily(terrain::grass, terrain::GRASS_VARIANTS);
        addFamily(terrain::dirt,  terrain::DIRT_VARIANTS);
        addFamily(terrain::rock,  terrain::ROCK_VARIANTS);
        addFamily(terrain::ore,   terrain::ORE_VARIANTS);

        assets.preloadTextures(paths);

        // Fonts: once the engine has a Font type -> assets.loadFont(font::PIXEL);
    }
}
