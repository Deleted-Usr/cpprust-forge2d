#pragma once

#include <forge2d/core/Game.hpp>

#include "Player.hpp"
#include "Enemy.hpp"

class ShmupGame final : public forge::Game
{
    public:
        void onStart(forge::AssetManager& assetManager) override;
        void onStop() override;

        void update(float deltaTime, const forge::InputManager& input) override;
        void render(forge::Renderer2D& renderer) override;

    private:
        Player player = {};
        std::vector<Enemy> enemies;

        forge::time::Clock enemySpawnClock;
};
