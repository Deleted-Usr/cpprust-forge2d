#include "Enemy.hpp"

#include <forge2d/core/Screen.hpp>
#include <forge2d/math/Math.hpp>

Enemy::Enemy()
{
    // Random Spawn
    position.x = static_cast<float>(forge::math::randomInt(20, forge::Screen::width() - 35));
    position.y = -20;

    // Random Speed
    speed = static_cast<float>(forge::math::randomDouble(150, 300));
}

void Enemy::update(float deltaTime)
{
    position.y += speed * deltaTime;
}

void Enemy::render(forge::Renderer2D &renderer)
{
    renderer.drawCircle(
        forge::math::Vec2 { position.x, position.y },
        15,
        forge::colours::Red
    );
}
