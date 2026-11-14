#include "ShmupGame.hpp"

#include <forge2d/graphics/Renderer2D.hpp>

void ShmupGame::onStart(forge::AssetManager &assetManager)
{

}

void ShmupGame::onStop()
{

}

void ShmupGame::update(float deltaTime, const forge::InputManager& input)
{
    player.update(deltaTime, input);
}

void ShmupGame::render(forge::Renderer2D& renderer)
{
    player.render(renderer);
}
