#include "Player.hpp"

#include <forge2d/core/Screen.hpp>

void Player::update(float deltaTime, const forge::InputManager& input)
{
    for (auto& bullet : bullets)
    {
        bullet.update(deltaTime);
    }

    if (position.x < 5) position.x = 5;
    if (position.x + width > forge::Screen::width() - 5) position.x = 395 - width;

    if (input.isKeyDown(forge::Key::A)) position.x -= speed * deltaTime;
    if (input.isKeyDown(forge::Key::D)) position.x += speed * deltaTime;

    if (input.isMouseButtonDown(forge::MouseButton::Left) || input.isKeyDown(forge::Key::Space))
    {
        if (shootClock.getElapsedTime().asSeconds() > fireRate)
        {
            bullets.emplace_back(Bullet({ position.x + 5, position.y }));
            shootClock.restart();
        }
    }
}

void Player::render(forge::Renderer2D& renderer)
{
    for (auto& bullet : bullets)
    {
        bullet.render(renderer);
    }

    renderer.drawRect(
        forge::math::Transform2D {
            .position = position,
            .scale    = { width, height },
        },
        forge::colours::Black
    );
}
