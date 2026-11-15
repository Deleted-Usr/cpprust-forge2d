#pragma once

#include <forge2d/graphics/Renderer2D.hpp>

class Enemy
{
    public:
        Enemy();

        void update(float deltaTime);
        void render(forge::Renderer2D& renderer);

    private:
        forge::math::Vec2 position;
        float speed;
};
