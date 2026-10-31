// Game.hpp
#pragma once
#include "forge2d/assets/AssetManager.hpp"

namespace forge
{
    class Renderer2D;

    class Game
    {
        public:
            virtual ~Game() = default;

            virtual void onStart(AssetManager& assetManager) {}
            virtual void onStop()  {}

            virtual void update(float deltaTime)      = 0;
            virtual void render(Renderer2D& renderer) = 0;
    };
}
