#include "PlatformerGame.hpp"

#include <forge2d/graphics/Renderer2D.hpp>

#include <iostream>

void PlatformerGame::onStart()
{
    std::cout << "Platformer Started!" << std::endl;
}

float x = 0;
float y = 0;

void PlatformerGame::update(float deltaTime)
{
    x += 0.001;

    // Eventually:
    //
    // if (Input::isKeyDown(Key::D))
    // {
    //     position_.x += 200.0f * deltaTime;
    // }
}

void PlatformerGame::render(forge::Renderer2D& renderer)
{
    renderer.drawRect(
        forge::math::Transform2D {
            .position = { x, 0.0f },
            .rotation = 0.0f,
            .scale    = { 1.0f, 1.0f }
        },
        { .r = 0.0f, .g = 1.0f, .b = 0.5f }
    );

    // Eventually:
    //
    // renderer.drawSprite(
    //     playerTexture_,
    //     {position_.x, position_.y}
    // );
}

void PlatformerGame::onStop()
{
    std::cout << "Platformer stopped!" << std::endl;
}
