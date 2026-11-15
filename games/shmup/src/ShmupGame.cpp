#include "ShmupGame.hpp"

#include <forge2d/graphics/Renderer2D.hpp>

void ShmupGame::onStart(forge::AssetManager &assetManager)
{

}

void ShmupGame::onStop()
{

}

void ShmupGame::update(float deltaTime, const forge::InputManager& input)
{
    player.update(deltaTime, input);

    if (enemySpawnClock.getElapsedTime().asSeconds() >= 1.0)
    {
        enemies.emplace_back();
        enemySpawnClock.restart();
    }

    for (auto& enemy : enemies)
    {
        enemy.update(deltaTime);
    }
}

void ShmupGame::render(forge::Renderer2D& renderer)
{
    player.render(renderer);

    for (auto& enemy : enemies)
    {
        enemy.render(renderer);
    }
}
