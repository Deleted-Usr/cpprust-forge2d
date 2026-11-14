#pragma once
#include "forge2d/graphics/Renderer2D.hpp"

class Bullet
{
    public:
        Bullet(forge::math::Vec2 position);

        void update(float deltaTime);
        void render(forge::Renderer2D& renderer);

        // Getters
        forge::math::Vec2 getPosition() { return position; }
        bool isActive() const           { return active; }

        // Setters
        void setActive(bool active) { this->active = active; }

    private:
        forge::math::Vec2 position = {};
        float speed = 300;

        bool active = true;
};
