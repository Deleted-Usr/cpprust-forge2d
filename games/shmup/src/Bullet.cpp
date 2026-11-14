#include "Bullet.hpp"

Bullet::Bullet(forge::math::Vec2 position)
{
    this->position = position;
}

void Bullet::update(float deltaTime)
{
    if (!active) return;

    position.y -= speed * deltaTime;
}

void Bullet::render(forge::Renderer2D& renderer)
{
    if (!active) return;

    renderer.drawCircle(
        position,
        4,
        forge::colours::Yellow
    );
}
