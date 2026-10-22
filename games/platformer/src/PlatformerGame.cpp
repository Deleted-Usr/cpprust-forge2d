#include "PlatformerGame.hpp"

#include <forge2d/graphics/Renderer2D.hpp>

#include <iostream>

void PlatformerGame::onStart()
{
    std::cout << "Platformer Started!" << std::endl;
}

void PlatformerGame::update(float deltaTime)
{
    std::cout << "platformer tick" << std::endl;

    // Eventually:
    //
    // if (Input::isKeyDown(Key::D))
    // {
    //     position_.x += 200.0f * deltaTime;
    // }
}

void PlatformerGame::render(forge::Renderer2D& renderer)
{
    std::cout << "platformer draw" << std::endl;

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
