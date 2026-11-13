#include "PlatformerGame.hpp"

#include <forge2d/graphics/Renderer2D.hpp>

#include <iostream>

void PlatformerGame::onStart(forge::AssetManager &assetManager)
{
    std::cout << "Platformer Started!" << std::endl;
}

void PlatformerGame::update(float deltaTime, const forge::InputManager& input)
{
    if (input.isKeyDown(forge::Key::D)) position_.x += 200.0f * deltaTime;
    if (input.isKeyDown(forge::Key::A)) position_.x -= 200.0f * deltaTime;
}

void PlatformerGame::render(forge::Renderer2D& renderer)
{
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
