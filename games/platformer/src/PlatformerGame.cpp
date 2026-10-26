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
        { .x = x, .y = y },
        { .x = 0.3f, .y = 0.2f },
        { .r = 0.0f, .g = 1.0f, .b = 0.0f }
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
