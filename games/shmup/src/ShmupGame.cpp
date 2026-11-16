#include "ShmupGame.hpp"

#include <forge2d/core/Screen.hpp>

void ShmupGame::onStart(forge::AssetManager &assetManager)
{
    // Enemies start above the screen; remove them only after they leave below it.
    std::erase_if(enemies, [](const Enemy& enemy)
    {
        return enemy.getPosition().y - enemy.getRadius() > forge::Screen::height();
    });

    auto& bullets = player.getBullets();
    std::erase_if(bullets, [](const Bullet& bullet)
    {
        return !bullet.isActive() || bullet.getPosition().y + bullet.getRadius() < 0;
    });

    handleCollisions();

    std::erase_if(bullets, [](const Bullet& bullet) { return !bullet.isActive(); });
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

    handleCollisions();
}

void ShmupGame::render(forge::Renderer2D& renderer)
{
    player.render(renderer);

    for (auto& enemy : enemies)
    {
        enemy.render(renderer);
    }
}

void ShmupGame::handleCollisions()
{
    for (auto& bullet : player.getBullets())
    {
        if (!bullet.isActive()) continue;

        const auto hit = std::find_if(enemies.begin(), enemies.end(), [&bullet](const Enemy& enemy)
        {
            const auto bulletPosition = bullet.getPosition();
            const auto enemyPosition  = enemy.getPosition();

            const float dx = bulletPosition.x - enemyPosition.x;
            const float dy = bulletPosition.y - enemyPosition.y;

            const float radius = bullet.getRadius() + enemy.getRadius();
            return dx * dx + dy * dy <= radius * radius;
        });

        if (hit != enemies.end())
        {
            bullet.setActive(false);
            enemies.erase(hit);
        }
    }
}
