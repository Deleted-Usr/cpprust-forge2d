#pragma once

#include <forge2d/core/Game.hpp>
#include <forge2d/math/Vec2.hpp>

class PlatformerGame final : public forge::Game
{
    public:
        void onStart(forge::AssetManager& assetManager) override;
        void onStop()  override;

        void update(float deltaTime) override;
        void render(forge::Renderer2D& renderer) override;

    private:
        forge::math::Vec2 position_ = { 100.f, 100.f };
};
