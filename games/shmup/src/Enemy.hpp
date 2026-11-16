#pragma once

#include <forge2d/graphics/Renderer2D.hpp>

class Enemy
{
    public:
        Enemy();

        void update(float deltaTime);
        void render(forge::Renderer2D& renderer);

        forge::math::Vec2 getPosition() const { return position; }
        static constexpr float getRadius() { return 15.0f; }

    private:
        forge::math::Vec2 position;
        float speed;
};
