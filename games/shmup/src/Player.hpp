#pragma once

#include <vector>

#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/input/InputManager.hpp>

#include "Bullet.hpp"
#include "forge2d/time/Clock.hpp"

class Player
{
    public:
        Player() = default;

        void update(float deltaTime, const forge::InputManager& input);
        void render(forge::Renderer2D& renderer);

    private:
        forge::math::Vec2 position { 190, 600 };
        float speed = 300;

        float width = 10, height = 30;

        std::vector<Bullet> bullets;
        forge::time::Clock shootClock;
        float fireRate = 0.2f;
};
